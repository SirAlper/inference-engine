# Building

## Requirements

| Tool | Version used | Notes |
| --- | --- | --- |
| C++ compiler | GCC 15.2 | Any C++20 compiler should work; only GCC is tested |
| CMake | 4.2.1 | ≥ 3.25 is needed for presets version 6 |
| Ninja | 1.13.2 | Generator used by the presets |
| Protobuf | protoc 33.1 | Found in CONFIG mode (pulls in abseil) |
| GoogleTest | — | Only needed when `ENGINE_BUILD_TESTS=ON` |
| Google Benchmark | — | Only needed when `ENGINE_BUILD_BENCHMARKS=ON` |

## Windows (MSYS2 UCRT64)

This is the primary development environment.

1. Install [MSYS2](https://www.msys2.org/), then install the toolchain and libraries from a UCRT64 shell:

   ```sh
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja
   pacman -S mingw-w64-ucrt-x86_64-protobuf mingw-w64-ucrt-x86_64-gtest mingw-w64-ucrt-x86_64-benchmark
   ```

2. Add `C:\msys64\ucrt64\bin` to `PATH`. The compiler needs the DLLs in that directory at run time.

3. Use MSYS2's CMake (`C:/msys64/ucrt64/bin/cmake.exe`). If another CMake is installed, for example in `C:\Program Files\CMake`, make sure it is not the one being used: it will not find the MSYS2 packages reliably.

## Configure, build, test

```sh
cmake --preset debug          # configure into build/debug
cmake --build --preset debug  # build
ctest --preset debug          # run tests
```

Replace `debug` with `release` for an optimized build in `build/release`. Run benchmarks only from release builds.

### Presets

| Preset | Build type | Directory |
| --- | --- | --- |
| `debug` | Debug | `build/debug` |
| `release` | Release | `build/release` |

The presets set the compiler (`C:/msys64/ucrt64/bin/g++.exe`) and Ninja paths explicitly. For now they also turn `ENGINE_BUILD_TESTS` and `ENGINE_BUILD_BENCHMARKS` off, because the `tests/` and `benchmarks/` directories are not set up yet.

Options can be overridden at configure time:

```sh
cmake --preset release -DENGINE_ENABLE_AVX2=OFF
```

## VS Code

The repository includes `.vscode/settings.json`, which:

- points the CMake Tools extension at MSYS2's CMake (`cmake.cmakePath`);
- always uses `CMakePresets.json` (`cmake.useCMakePresets`);
- makes the C/C++ extension take include paths and flags from CMake Tools.

After opening the folder, select the `debug` preset in the status bar. `compile_commands.json` is generated in the build directory for IntelliSense and clangd.

## Troubleshooting

**"The CMake cache was created by a different CMake" or similar cache errors.**
The build directory was configured by another CMake installation. Delete `build/<preset>` and configure again.

**`Could NOT find ZLIB` while configuring.**
This came from configuring with a non-MSYS2 CMake. With MSYS2's CMake, zlib is found automatically. If it still appears, install `mingw-w64-ucrt-x86_64-zlib`.

**The compiler check fails with no error message.**
`g++` could not load its DLLs. This happens when another directory earlier in `PATH` (for example Git for Windows' `mingw64/bin`) provides incompatible versions. Make sure `C:\msys64\ucrt64\bin` comes before such directories, or build from PowerShell or the MSYS2 UCRT64 shell.
