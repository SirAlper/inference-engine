# Building

## Requirements

| Tool | Version used | Notes |
| --- | --- | --- |
| C++ compiler | GCC 15.2 (Windows), Apple clang 21 (macOS) | Any C++20 compiler should work |
| CMake | 4.2.1 | ≥ 3.25 is needed for presets version 6 |
| Ninja | 1.13.2 | Generator used by the presets |
| Protobuf | protoc 33.1 | Found in CONFIG mode (pulls in abseil) |
| GoogleTest | 1.17.0 | Found in CONFIG mode; only needed when `ENGINE_BUILD_TESTS=ON` |
| Google Benchmark | — | Only needed when `ENGINE_BUILD_BENCHMARKS=ON` |

## macOS (Homebrew)

1. Install the Xcode Command Line Tools (`xcode-select --install`) and keep them up to date through Software Update. The Command Line Tools and the default SDK must come from the same release; see [Troubleshooting](#troubleshooting).

2. Install the tools and libraries:

   ```sh
   brew install cmake ninja protobuf googletest
   ```

The `macos-*` presets use Apple clang and add `/opt/homebrew` to `CMAKE_PREFIX_PATH`. On Apple Silicon `ENGINE_ENABLE_AVX2` defaults to `OFF`.

## Windows (MSYS2 UCRT64)

1. Install [MSYS2](https://www.msys2.org/), then install the toolchain and libraries from a UCRT64 shell:

   ```sh
   pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja
   pacman -S mingw-w64-ucrt-x86_64-protobuf mingw-w64-ucrt-x86_64-gtest mingw-w64-ucrt-x86_64-benchmark
   ```

2. Add `C:\msys64\ucrt64\bin` to `PATH`. The compiler needs the DLLs in that directory at run time.

3. Use MSYS2's CMake (`C:/msys64/ucrt64/bin/cmake.exe`). If another CMake is installed, for example in `C:\Program Files\CMake`, make sure it is not the one being used: it will not find the MSYS2 packages reliably. In VS Code, set `"cmake.cmakePath": "C:/msys64/ucrt64/bin/cmake.exe"` in your user settings.

## Configure, build, test

Presets are named `<platform>-<build type>`; only the ones for the current host are shown. On macOS:

```sh
cmake --preset macos-debug          # configure into build/macos-debug
cmake --build --preset macos-debug  # build
ctest --preset macos-debug          # run tests
```

On Windows use `windows-debug`. Replace `debug` with `release` for an optimized build. Run benchmarks only from release builds.

### Presets

| Preset | Build type | Directory |
| --- | --- | --- |
| `macos-debug` | Debug | `build/macos-debug` |
| `macos-release` | Release | `build/macos-release` |
| `windows-debug` | Debug | `build/windows-debug` |
| `windows-release` | Release | `build/windows-release` |

The Windows presets set the compiler (`C:/msys64/ucrt64/bin/g++.exe`) and Ninja paths explicitly; the macOS presets use `clang++` and Homebrew. Tests are built by default. For now the presets turn `ENGINE_BUILD_BENCHMARKS` off, because the `benchmarks/` directory is not set up yet.

## Tests

Unit tests use GoogleTest and live in `tests/`. They are built into a single executable, `engine_tests`, linked against `engine_core` and `GTest::gtest_main`. `gtest_discover_tests` registers every `TEST` with CTest after each build, so new tests show up in `ctest` without reconfiguring.

```sh
ctest --preset macos-debug                    # run all tests
ctest --preset macos-debug -R StorageTest     # run tests whose name matches a regex
build/macos-debug/tests/engine_tests          # run the executable directly (engine_tests.exe on Windows)
```

To add a test file, create it in `tests/` and add it to the `add_executable(engine_tests ...)` list in `tests/CMakeLists.txt`.

Options can be overridden at configure time:

```sh
cmake --preset macos-release -DENGINE_BUILD_TESTS=OFF
```

## VS Code

The repository includes `.vscode/settings.json`, which:

- always uses `CMakePresets.json` (`cmake.useCMakePresets`);
- makes the C/C++ extension take include paths and flags from CMake Tools.

After opening the folder, select the `macos-debug` or `windows-debug` preset in the status bar. `compile_commands.json` is generated in the build directory for IntelliSense and clangd.

## Troubleshooting

**"The CMake cache was created by a different CMake" or similar cache errors.**
The build directory was configured by another CMake installation. Delete `build/<preset>` and configure again.

**macOS: `ld: tapi error: malformed file ... unknown architecture` while configuring.**
The default SDK is newer than the installed Command Line Tools (for example SDK 27 with Command Line Tools 26), so the linker cannot read it. Install the matching Command Line Tools from System Settings → Software Update (or `softwareupdate --list` / `softwareupdate --install "<label>"`), then delete `build/<preset>` and configure again. As a temporary workaround, point at the older SDK: `export SDKROOT=/Library/Developer/CommandLineTools/SDKs/MacOSX26.sdk`.

**Windows: `Could NOT find ZLIB` while configuring.**
This came from configuring with a non-MSYS2 CMake. With MSYS2's CMake, zlib is found automatically. If it still appears, install `mingw-w64-ucrt-x86_64-zlib`.

**Windows: the compiler check fails with no error message.**
`g++` could not load its DLLs. This happens when another directory earlier in `PATH` (for example Git for Windows' `mingw64/bin`) provides incompatible versions. Make sure `C:\msys64\ucrt64\bin` comes before such directories, or build from PowerShell or the MSYS2 UCRT64 shell.
