# ---------------------------------------------------------
#  engine_core: all engine modules (tensor, loader, graph, ops, runtime)
# ---------------------------------------------------------
add_library(engine_core STATIC
    tensor/storage.cpp
    tensor/tensor.cpp
)

# Allows #include "tensor/storage.hpp"; PUBLIC so that tests/benchmarks
# linking against engine_core see it as well.
target_include_directories(engine_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_features(engine_core PUBLIC cxx_std_20)

# Warnings apply only to engine_core's own sources, not to its consumers.
target_link_libraries(engine_core PRIVATE engine_warnings)

# SIMD flags are PRIVATE: intrinsics must only be used in .cpp files, so that
# inline functions in headers are not compiled with different flags (ODR violation).
if(ENGINE_ENABLE_AVX2)
    target_compile_options(engine_core PRIVATE -mavx2 -mfma)
    target_compile_definitions(engine_core PRIVATE ENGINE_HAS_AVX2=1)
endif()

# TODO: in the ONNX loader step:
# target_link_libraries(engine_core PRIVATE onnx_proto)
