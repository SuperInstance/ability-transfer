### **1. Module System: CUDAClaw vs. ZeroClaw Abstractions**  
The Claws architecture uses a **hierarchical, interface-driven module system** where core agent logic is shared, and hardware-specific "backends" are swapped based on Claw type. Modules are decoupled via abstract base classes (ABCs) or traits, enabling dynamic loading of GPU/CPU optimizations.


#### **Core Shared Modules**  
These are agnostic to hardware and shared across all Claw types:  
- **Agent Core**: Manages task queues, yoke state, and skill registration.  
- **Yoke Manager**: Handles serialization/deserialization of the yoke (see §3).  
- **Fleet Gateway**: Interface for communicating with the FleetDirectory (task routing, yoke transfer).  
- **Skill Registry**: Stores agent capabilities (e.g., `FLUX_inference`, `report_generation`) as versioned function signatures.  


#### **Hardware-Specific Backends**  
These are swapped between CUDAClaw and ZeroClaw via a **module manifest** (JSON/YAML) that maps abstract interfaces to concrete implementations. Below are critical differentiating modules:

| **Interface**               | **CUDAClaw Implementation**                          | **ZeroClaw Implementation**                          |  
|------------------------------|------------------------------------------------------|------------------------------------------------------|  
| `IExecutionEngine`           | `CUDAKernelExecutor`                                  | `CPULoopExecutor` (multi-threaded with `concurrent.futures`) |  
|                              | - Launches CUDA kernels for parallel batch processing (e.g., `cudaLaunchKernel`).<br>- Uses tensor cores for FP8/INT8 matrix multiplies (via cuBLAS/cuDNN). | - Serial/parallel execution via CPU threads (AVX/Neon optimized).<br>- Falls back to OpenBLAS for dense linear algebra. |  
| `ITensorHandler`             | `CUDATensor`                                          | `CPUDenseTensor`                                      |  
|                              | - Manages GPU memory (pinned/device buffers via `cudaMalloc`).<br>- Supports tensor core fusion (e.g., `cuDNN_conv2d_fwd`). | - Uses CPU-resident tensors (pageable memory).<br>- Optimized for low memory footprint (e.g., sparse tensor storage). |  
| `IMemoryAllocator`           | `CUDAPinnedAllocator`                                 | `CPUPageAllocator`                                    |  
|                              | - Allocates pinned GPU memory for high-bandwidth CPU-GPU transfers.<br>- Automatically frees device memory on task completion. | - Uses OS page-based allocation for CPU efficiency.<br>- Limits memory usage via `RLIMIT_AS` (for edge devices like Raspberry Pi). |  
| `IParallelScheduler`         | `CUDAKernelScheduler`                                 | `CPUBatchScheduler`                                   |  
|                              | - Maps task batches to GPU grid/block dimensions (e.g., 256 threads/block for small batches).<br>- Prioritizes tensor core-optimized tasks. | - Splits batches across CPU cores (e.g., 4 cores = 4 parallel batches).<br>- Throttles to avoid CPU overload on low-power devices. |  


#### **Module Loading Workflow**  
At boot, a `ModuleLoader` reads a `claw_manifest.json` (hardware-specific) to resolve interfaces to implementations:  
```json
// CUDAClaw manifest (Jetson/data center GPU)
{
  "execution_engine": "claws.backends.cuda.CUDAKernelExecutor",
  "tensor_handler": "claws.backends.cuda.CUDATensor",
  "memory_allocator": "claws.backends.cuda.CUDAPinnedAllocator"
}

// ZeroClaw manifest (VPS/Raspberry Pi)
{
  "execution_engine": "claws.backends.cpu.CPULoopExecutor",
  "tensor_handler": "claws.backends.cpu.CPUDenseTensor",
  "memory_allocator": "claws.backends.cpu.CPUPageAllocator"
}
```  
The `ModuleLoader` uses dependency injection to inject the correct backend into the core `Agent` class (e.g., `agent.execution_engine = CUDAKernelExecutor()`).


### **2. HybridClaw: Hardware Detection & Dynamic Switching**  
HybridClaw detects hardware at boot and loads the appropriate backend modules. For **runtime switching** (e.g., moving from GPU to CPU), it uses a two-phase process: *hardware probing* → *module reloading* → *yoke transfer*.


#### **Boot-Time Hardware Detection**  
On startup, HybridClaw runs a `HardwareProbe` that checks for:  
- **GPU Presence**: Uses `nvidia-smi` (Jetson) or `cudaGetDeviceCount()` (data center) to detect NVIDIA GPUs.  
- **CPU Capabilities**: Uses `cpuinfo` (Python) to check for AVX-512, Neon, or other optimizations.  
- **Memory Constraints**: Queries `sysinfo` (Linux) for RAM/swap limits (critical for edge devices).  

The probe returns a `HardwareSpec` object:  
```python
class HardwareSpec:
    gpu_present: bool = False
    gpu_model: str = "none"  # e.g., "Jetson Orin", "A100"
    cpu_cores: int = 1
    cpu_arch: str = "x86_64"
    ram_gb: float = 4.0
```  

The `ModuleLoader` uses `HardwareSpec` to select the manifest (CUDAClaw if `gpu_present=True`, else ZeroClaw).


#### **Runtime Switching Workflow**  
When an agent needs to move between Claw types (e.g., GPU inference → CPU reporting), HybridClaw triggers:  
1. **Yoke Freeze**: The source agent pauses task execution and locks the yoke state (via optimistic locking with a `transfer_version` counter).  
2. **Target Provisioning**: The FleetGateway queries the FleetDirectory for a target Claw instance matching the new hardware requirements (e.g., "CPU-only, <100ms latency").  
3. **Module Reload**: The target agent loads the appropriate backend modules (e.g., ZeroClaw if the target is a VPS).  
4. **Yoke Transfer**: The frozen yoke is serialized and sent to the target (see §3).  
5. **State Resumption**: The target agent deserializes the yoke and resumes task execution with the new backend.  


### **3. Yoke Transfer Protocol**  
The yoke is the complete agent state, serialized into a versioned binary format for efficient transfer. The protocol supports **forking** (shared state with delta updates) and **cloning** (full state copy) via a content-addressable storage (CAS) layer.


#### **Yoke Structure**  
The yoke is defined as a **Protocol Buffer (Protobuf)** schema (`yoke.proto`) for speed and compatibility:  
```protobuf
syntax = "proto3";

message Yoke {
  string agent_id = 1;          // UUID of the agent
  string schema_version = 2;    // e.g., "1.2.0" for backward compatibility
  string repo_commit = 3;       // Git commit hash of the agent's codebase
  map<string, bytes> context = 4; // Key-value store (e.g., "user_query" → JSON bytes)
  repeated Task tasks = 5;      // Ongoing tasks (status, params, checkpoints)
  SkillRegistry skills = 6;     // Registered skills (function signatures)
  bytes memory_snapshot = 7;    // Vector DB embeddings (FAISS/Annoy serialized)
  uint64 transfer_version = 8;  // Optimistic lock counter
}

message Task {
  string id = 1;
  string status = 2;            // "pending", "running", "completed"
  map<string, bytes> params = 3; // Task-specific data (e.g., "input_tensor" → tensor bytes)
}

message SkillRegistry {
  map<string, SkillSignature> skills = 1;
}

message SkillSignature {
  string name = 1;
  repeated string inputs = 2;
  string output_type = 3;
}
```  


#### **Transfer Modes: Fork vs. Clone**  
- **Cloning**: Used when the target hardware belongs to the **same agent** (e.g., moving from a Jetson to a backup VPS). The entire yoke is serialized to Protobuf and sent via gRPC streaming (chunked transfer with SHA-256 checksums per chunk).  
- **Forking**: Used when the target belongs to **another agent** (e.g., offloading reporting to a shared VPS pool). Only the **delta** (changed context/tasks) is transferred, using a CAS (e.g., IPFS or S3) to reference shared assets (e.g., `repo_commit`, `memory_snapshot`). The target agent merges the delta into a base yoke stored in the CAS.  


#### **Integrity & Consistency**  
- **Checksums**: Every yoke chunk includes a SHA-256 hash; the receiver verifies all chunks before resuming.  
- **Optimistic Locking**: The `transfer_version` counter prevents race conditions—if the source agent modifies the yoke during transfer, the target rejects the outdated version, and the process retries.  
- **Atomicity**: Transfers use gRPC’s "idempotent" streaming to ensure no partial state is left on the target.  


### **4. FLUX Opcodes: Adaptive Behavior by Claw Type**  
FLUX bytecode includes **backend hints** that Claw types use to select optimized implementations. Opcodes are grouped by hardware suitability, with fallback logic for cross-Claw compatibility.


#### **Opcode Classification**  
| **Opcode Category** | **CUDAClaw Behavior**                          | **ZeroClaw Behavior**                          |  
|----------------------|------------------------------------------------|------------------------------------------------|  
| **Parallel Batch**   | Maps to `CUDAKernelExecutor.launch()` with grid/block dimensions tuned for tensor cores (e.g., 32x32 blocks for 1024-thread batches). | Maps to `CPULoopExecutor.parallel_execute()` with batches split across CPU cores (e.g., 4 cores = 4 batches). |  
| **Tensor Operation** | Uses cuBLAS/cuDNN for matrix multiplies/convolutions (e.g., `FLUX_OP_MATMUL` → `cublasGemmEx`). | Uses CPU-optimized libraries (e.g., BLIS for `FLUX_OP_MATMUL`, ONNX Runtime for quantized tensors). |  
| **Memory Transfer**  | Uses `cudaMemcpyAsync` for pinned-host ↔ device memory transfers. | Uses zero-copy CPU memory (avoids `memcpy` overhead). |  
| **Latency-Critical** | Prioritizes tensor core acceleration (e.g., `FLUX_OP_FAST_INFERENCE`). | Uses single-threaded execution with CPU cache optimization (e.g., `O2` compiler flags). |  


#### **Opcode Metadata**  
Each FLUX opcode includes a `backend_hints` field to guide Claw behavior:  
```json
// Example: FLUX_OP_PARALLEL_BATCH opcode
{
  "opcode": 0x01,
  "backend_hints": {
    "parallelism": "high",       // Triggers GPU kernel launch in CUDAClaw
    "memory": "gpu_shared",      // Uses pinned memory in CUDAClaw; pageable in ZeroClaw
    "fallback": "cpu_serial"     // If GPU unavailable, use ZeroClaw's serial execution
  },
  "params": { "batch_size": 256 }
}
```  

The `IExecutionEngine` in each Claw type uses a lookup table to map opcodes + hints to implementations:  
```python
# CUDAClaw's execution engine lookup
OPCODE_MAP = {
    0x01: lambda hints, params: cuda_launch_parallel_batch(params["batch_size"]),
    0x02: lambda hints, params: cublas_matmul(params["a"], params["b"]),
    # ...
}

# ZeroClaw's execution engine lookup
OPCODE_MAP = {
    0x01: lambda hints, params: cpu_parallel_batch(params["batch_size"]),
    0x02: lambda hints, params: blis_matmul(params["a"], params["b"]),
    # ...
}
```  


### **5. Mapping to Existing Fleet (Oracle1 & JetsonClaw1)**  
The existing fleet—**Oracle1 (ZeroClaw on Oracle VPS)** and **JetsonClaw1 (CUDAClaw on Jetson)**—is integrated via a **FleetDirectory** (gRPC service) that tracks agent capabilities and routes tasks.


#### **FleetDirectory Registration**  
Each agent registers its hardware/claw type with the FleetDirectory on boot:  
```grpc
// Service: FleetDirectory
rpc RegisterAgent(AgentSpec) returns (RegistrationACK);

message AgentSpec {
  string agent_id = 1;
  string claw_type = 2;          // "ZeroClaw" or "CUDAClaw"
  HardwareSpec hardware = 3;     // From §2.1
  string endpoint = 4;           // gRPC address for yoke transfer
}
```  

For example:  
- **Oracle1**: `claw_type="ZeroClaw"`, `hardware={cpu_cores=4, ram_gb=8, gpu_present=False}`, `endpoint="oracle1.claws.fleet:50051"`.  
- **JetsonClaw1**: `claw_type="CUDAClaw"`, `hardware={gpu_model="Jetson Orin", tensor_cores=8, ram_gb=16}`, `endpoint="jetson1.claws.fleet:50051"`.  


#### **Task Routing & Yoke Transfer Example**  
A workflow for a "sensor data inference → reporting" task:  
1. **Task Submission**: A user submits a task: `{"type": "inference", "batch_size": 1024, "next_step": "report"}`.  
2. **FleetManager Routing**: The FleetManager queries the FleetDirectory for agents with `claw_type="CUDAClaw"` and `gpu_model="Jetson Orin"` → selects **JetsonClaw1**.  
3. **Execution**: JetsonClaw1 runs the inference task using `CUDAKernelExecutor` and `CUDATensor`, storing results in the yoke’s `context` field.  
4. **Yoke Transfer Trigger**: When the inference completes, the task’s `next_step` flag triggers a transfer to a CPU agent. The FleetManager selects **Oracle1** (low latency, ZeroClaw).  
5. **Transfer**: JetsonClaw1 serializes the yoke (Protobuf), sends it via gRPC to Oracle1, which deserializes and resumes the task (generating a report using `CPULoopExecutor`).  


#### **Edge Case: ZeroClaw → CUDAClaw Transfer**  
If Oracle1 needs to offload a large batch task to JetsonClaw1:  
- The yoke’s `repo_commit` is checked out on JetsonClaw1 (via git pull).  
- The `memory_snapshot` (vector embeddings) is transferred via CAS (S3) to avoid reprocessing.  
- JetsonClaw1 loads the CUDAClaw backends and resumes the task with GPU acceleration.  


### **Key Implementation Artifacts**  
- **Module Manifests**: JSON files per Claw type, stored in `claws/manifests/`.  
- **Yoke Protobuf**: Compiled to Python/Golang bindings for serialization.  
- **FleetDirectory**: gRPC service with etcd backend for agent registration.  
- **HardwareProbe**: Cross-platform library (Python) using `pynvml`, `cpuinfo`, and `sysinfo`.  

This design ensures seamless hardware adaptation, efficient state transfer, and consistent performance across the Claws fleet.
