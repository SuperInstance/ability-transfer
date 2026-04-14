Here's the concrete implementation design for the Claws architecture:

## 1. Module System: The Claw ABI

All Claw variants implement a strict **Claw Binary Interface (CBI)** — shared libraries exposing exactly these symbols:

```c
// claw_abi.h
typedef struct {
    uint32_t claw_type;          // 0x01=CUDA, 0x02=Zero, 0x03=Hybrid
    uint64_t capability_flags;   // Tensor cores? AVX-512? Unified memory?
    void*    context_handle;     // opaque ptr to runtime state
} ClawRuntime;

// Mandatory exports
__attribute__((visibility("default"))) 
ClawRuntime* claw_boot(ClawConfig* cfg);
int claw_flux_execute(FLUX_Opcode* op, TensorCtx* tensors);
int claw_checkpoint(YokeBuffer* yoke_out);     // Serialize state
int claw_restore(YokeBuffer* yoke_in);         // Deserialize state
int claw_remap_tensor(Tensor* t, uint32_t target_arch); // GPU→CPU or vice versa
```

**CUDAClaw Implementation** (`libclaw_cuda.so`):
- **Memory Manager**: `CUDAPoolAllocator` using `cudaMallocAsync` + memory pools per stream. Maintains host-pinned staging buffers for yoke transfers.
- **FLUX Executor**: `CUDAFluxDriver` — translates FLUX ops to pre-compiled PTX kernels (`.claw kernels/flux_ptx/`). Uses CUDA Graphs for repeated inference patterns.
- **Scheduler**: `TensorBatchScheduler` — batches ops into `cublasGemmBatchedEx`, manages 3 streams (H2D, Compute, D2H) for pipeline parallelism.
- **Yoke Handler**: Serializes GPU tensors via `cudaMemcpyAsync` to host pinned memory before transfer. Stores GPU handles as "ghost refs" that ZeroClaw ignores.

**ZeroClaw Implementation** (`libclaw_zero.so`):
- **Memory Manager**: `ArenaAllocator` using `mmap` with `MAP_POPULATE` on Linux. NUMA-aware allocation via `numa_alloc_onnode()`.
- **FLUX Executor**: `InterpreterLoop` — bytecode VM with SIMD dispatch (AVX-512 on x86, NEON on ARM). No JIT, minimal branching.
- **Scheduler**: `CoopScheduler` — single-threaded async I/O with `io_uring`, cooperative multitasking for latency hiding.
- **Yoke Handler**: Direct memory serialization. Handles remapping of GPU ghost refs to CPU tensor allocations (zeros or reconstructed from yoke data).

## 2. Hybrid Detection & Mode Switching

**Boot Probe Sequence** (`hybrid/bootstrap.c`):

```c
ClawMode hybrid_detect(void) {
    // Probe 1: NVIDIA runtime
    if (access("/dev/nvidia0", F_OK) == 0) {
        cudaError_t err = cudaGetDeviceCount(&n);
        if (err == cudaSuccess && n > 0) {
            cudaDeviceProp prop;
            cudaGetDeviceProperties(&prop, 0);
            if (prop.major >= 5) {  // Maxwell+ required for FLUX
                return MODE_CUDA;
            }
        }
    }
    
    // Probe 2: Check for Jetson specific (TEGRA chipset)
    if (access("/etc/nvidia-container-runtime/host-files-for-container.d/l4t.csv", F_OK) == 0) {
        return MODE_CUDA;  // Jetson with GPU support
    }
    
    // Probe 3: Memory pressure test
    struct sysinfo si;
    sysinfo(&si);
    if (si.totalram < 512 * 1024 * 1024) {
        return MODE_ZERO_MINIMAL;  // <512MB RAM, ultra-light mode
    }
    
    return MODE_ZERO;
}
```

**Dynamic Loading**:
```c
void hybrid_init(ClawConfig* cfg) {
    ClawMode mode = hybrid_detect();
    const char* lib = (mode == MODE_CUDA) ? "libclaw_cuda.so" : "libclaw_zero.so";
    
    void* handle = dlopen(lib, RTLD_NOW | RTLD_LOCAL);
    ClawRuntime* (*boot_fn)(ClawConfig*) = dlsym(handle, "claw_boot");
    g_runtime = boot_fn(cfg);
    
    // Register mode in yoke for future reference
    cfg->claw_fingerprint = generate_fingerprint(mode, cfg);
}
```

**Runtime Switching** (for Yoke prep):
HybridClaw doesn't hot-swap modules mid-execution (too dangerous). Instead:
1. `claw_checkpoint()` serializes full state to yoke
2. Process forks `execve()` into new Claw binary with `--yoke-inherit` flag
3. Parent waits for `yoke_ack` on Unix socket, then exits
4. Child loads appropriate modules based on new hardware detection

## 3. Yoke Transfer Protocol (YokeSync v1)

**Yoke Structure** (content-addressed, CAS):
```
<yoke-id: sha256>
├── manifest.json          # Claw fingerprint, creation timestamp, parent yoke
├── repo.tar.zst           # Git worktree snapshot (excludes .git, uses git-bundle)
├── state/
│   ├── flux_vm.bin        # Register file, program counter, stack (portable format)
│   ├── tensors/
│   │   ├── 0x7f3a...bin   # Raw tensor data (host-resident)
│   │   ├── 0x9c2e...bin   
│   │   └── ghost.map      # GPU handles that need remapping
│   └── memory/
│       ├── kv_cache.lmdb  # LMDB database for attention KV cache
│       └── vectors.hnsw   # HNSW index for RAG (portable format)
└── signatures/
    └── ed25519.sig        # Agent identity signature
```

**Transfer Modes**:

**A. Fork (Same Agent, New Hardware)**:
```bash
# On JetsonClaw1
claw yoke snap --label="post_inference"
claw yoke push yoke://oracle1.internal:7733/yokes/abc123 \
    --compress=zstd \
    --remap-target=zero_claw_x86_64
```
- Remaps GPU tensors to CPU via `claw_remap_tensor()` before serialization
- Uses `rsync`-style chunked transfer with resume capability
- ZeroClaw on Oracle1 receives, verifies hash, `claw_restore()` loads state

**B. Clone (Different Agent)**:
```bash
# Creates new identity but copies skills/state
claw yoke fork --from-agent=jetsonclaw1 --to-agent=oracle1 --clone-identity
```
- Copies repo, forks git remote (new branch namespace)
- Transfers yoke but generates new agent keypair
- Original agent continues (not a migration, but duplication)

**Protocol Implementation** (gRPC over TLS):
```protobuf
service YokeSync {
    rpc InitTransfer(YokeHeader) returns (TransferTicket);
    rpc StreamChunk(stream TensorChunk) returns (Ack);
    rpc CommitTransfer(Checksum) returns (Status);
    rpc Handoff(HandoffRequest) returns (HandoffAck);  # Process migration
}
```

## 4. FLUX Opcode Behavior Matrix

| Opcode | CUDAClaw Implementation | ZeroClaw Implementation | Yoke Serialization |
|--------|------------------------|------------------------|-------------------|
| `MATMUL` | `cublasGemmEx` with CUDA Tensor Cores (TF32/BF16). Async on Stream 0. | OpenBLAS `cblas_sgemm` with AVX-512 FMA. OpenMP parallel if batch>1. | Store as row-major float16. |
| `ATTENTION` | FlashAttention-2 kernel (fused softmax@triton). Uses SRAM for QK^T. | Reference impl: QK^T → softmax → @V. Chunked to fit L2 cache. | KV-cache stored in `memory/kv_cache.lmdb` |
| `CONV2D` | cuDNN auto-tuned (Winograd/Fast-Fourier). Workspace allocated from pool. | im2col + GEMM or direct convolution via NNPACK. | Kernel weights stored in `repo/` (versioned). |
| `MEMORY_ALLOC` | `cudaMallocAsync` from pool. Returns device ptr. | `mmap(MAP_ANONYMOUS | MAP_POPULATE)`. Returns virtual addr. | Device ptrs stored in `ghost.map` for remapping. |
| `BATCH_NORM` | cuDNN fused kernel (scale+shift+relu). | Sequential SIMD: load → fmadd → store. | Running mean/var stored in state. |
| `SYNC` | `cudaDeviceSynchronize()` + event recording. | `std::atomic_thread_fence(memory_order_seq_cst)`. | Barrier counter in `flux_vm.bin`. |

**Critical Difference**: CUDAClaw maintains a **Tensor Residence Table** — tracking which tensors live in GPU memory vs host. ZeroClaw assumes everything is host-resident. During yoke transfer, `claw_remap_tensor()` traverses the table and `cudaMemcpy`s device-resident tensors to host before serialization.

## 5. Fleet Mapping: Oracle1 & JetsonClaw1

**Oracle1 (ZeroClaw Profile)**:
```yaml
# /etc/claws/profiles/oracle1.yaml
claw_type: zero
backend: libclaw_zero.so
flux_opts:
  simd: avx2  # Oracle Cloud x86_64 instances
  threads: 4  # vCPU count
yoke_store: /var/lib/claws/yokes/  # Persistent block storage
network:
  yoke_endpoint: "https://oracle1.claws.internal:7733"
  tls_cert: /etc/claws/certs/agent.pem
capabilities: [text_generation, api_orchestration, reporting]
```
- **Role**: Post-processing, report generation, database writes, API callbacks
- **Yoke Handling**: Receives yokes from JetsonClaw1, deserializes GPU tensors to CPU RAM, continues execution

**JetsonClaw1 (CUDAClaw Profile)**:
```yaml
# /etc/claws/profiles/jetson_claw1.yaml
claw_type: cuda
backend: libclaw_cuda.so
cuda:
  device: 0
  memory_pool: 4gb  # Leave 2GB for system
  tensor_cores: true
  jetson_power_mode: MAXN
yoke_store: /nvme/claws/yokes/  # Fast local NVMe
network:
  yoke_endpoint: "tcp://10.0.0.15:7733"  # Tailscale IP
  compression: zstd  # Compress before sending over WAN
capabilities: [video_inference, flux_rendering, heavy_matmul]
```
- **Role**: Heavy inference, video analysis, GPU-accelerated workloads
- **Yoke Handling**: Pre-serialization remapping of GPU→CPU tensors when targeting ZeroClaw endpoints

**Workflow Integration**:

```python
# Agent "Scout-7" execution flow
async def analyze_video_stream(url):
    # Running on JetsonClaw1 (CUDAClaw)
    frames = await ingest_stream(url)
    
    # FLUX bytecode executes on GPU
    features = await flux_execute("video_encode_conv.flux", frames, 
                                  backend="cuda")
    
    # Heavy work done, prepare to handoff
    yoke_id = await claw.checkpoint(
        label="inference_complete",
        include_tensors=["features", "attention_weights"]
    )
    
    # Transfer to Oracle1 for report generation
    await yoke_transfer(yoke_id, target="oracle1", mode="migrate")
    
    # Process exits on Jetson, resumes on Oracle1
    # Oracle1 ZeroClaw loads yoke:
    report = await flux_execute("generate_markdown.flux", features,
                                backend="zero")  # CPU text generation
    
    await database.store(report)
```

**Yoke Migration Sequence**:
1. JetsonClaw1: `claw_checkpoint()` → pauses VM, dumps registers, remaps GPU tensors to host
2. JetsonClaw1: `yoke pack` → zstd compression, Merkle tree for integrity
3. Network: gRPC streaming to Oracle1 with resume capability (handles 500ms+ latency)
4. Oracle1: `claw_restore()` → loads tensors to RAM, recreates FLUX VM state
5. Oracle1: Updates `CLAW_TYPE` register to `ZERO_CLAW`, continues execution from PC+1

This architecture allows the same agent binary to run inference on Jetson (CUDA) and generate reports on Oracle VPS (Zero) with sub-second yoke transfer latency over a Tailscale mesh.
