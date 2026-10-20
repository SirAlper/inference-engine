# Roadmap

Each milestone ends with something that can be verified: a test against a reference, or a model that runs end to end.

## 1. Tensor, Storage and naive operators — *in progress*

- [x] `Storage`: 64-byte aligned, non-copyable owner of raw memory.
- [x] `DType`: `float32` and `int64`, with `dtype_size`, `dtype_name` and the compile-time `dtype_of<T>` mapping.
- [x] `Tensor` basics: shape, row-major strides, `numel`/`nbytes`, shared storage plus element offset.
- [x] `Tensor::data<T>()`: typed element access with a run-time dtype check.
- [ ] `Tensor` factories (`zeros`, `from_vector`).
- [ ] Zero-copy views (reshape, transpose, slice), `is_contiguous()` and `contiguous()`.
- Reference (unoptimized) operators in `ops/`, written for clarity rather than speed. They serve as the correctness baseline for every later optimization.
- First unit tests with GoogleTest.

## 2. ONNX loader — *planned*

- Generate the `onnx_proto` library from `proto/onnx.proto`.
- Parse a `.onnx` file: graph inputs and outputs, nodes, attributes and initializers (weights).
- Convert initializers into engine tensors. Protobuf types stay inside `loader/`.

## 3. Graph and executor — *planned*

- Graph representation with topological ordering.
- A sequential executor that runs nodes in order.
- **Target:** an MNIST MLP runs end to end and its outputs match onnxruntime.

## 4. CNN operators — *planned*

- Conv, MaxPool / AveragePool, BatchNormalization and the other operators these models need.
- **Target:** ResNet-18 and MobileNet run end to end and match onnxruntime.

## 5. Optimization — *planned*

- Tiled GEMM.
- AVX2/FMA kernels.
- A custom thread pool.
- Operator fusion (for example Conv + BatchNorm + ReLU).
- A memory planner that reuses intermediate buffers.

Every optimized kernel is tested against the naive operator from milestone 1.

## 6. Benchmarks and report — *planned*

- Google Benchmark suite, run on release builds only.
- Side-by-side comparison with onnxruntime on the same machine and models.
- A written report of the results and the methodology.
