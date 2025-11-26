# OTPQCOM

**OTPQCOM** is a lightweight and modern **C++23 communication library** engineered for **secure, multi-peer, high-performance distributed systems**.  
It provides clean abstractions for socket communication, structured peer messaging, and efficient multi-party coordination—making it suitable for systems such as threshold cryptography, DKG, consensus protocols, and secure MPC workflows.

---

<p align="center">
  <img alt="build" src="https://img.shields.io/badge/build-passing-brightgreen?style=flat-square">
  <img alt="license" src="https://img.shields.io/badge/license-MIT-blue?style=flat-square">
  <img alt="standard" src="https://img.shields.io/badge/C%2B%2B-23-blue?style=flat-square">
</p>

---

## 🚀 Key Capabilities

- **Modern C++23 architecture**  
  RAII, `std::expected`, spans, structured bindings, and no raw memory ownership.

- **Versatile network I/O**  
  Configurable buffers, server/client channels, streaming operations.

- **Multi-peer communication layer (MPChannel)**  
  Simplifies N-party network coordination.

- **Reliable error handling**  
  Explicit and composable error propagation.

- **Cross-platform foundation**  
  Fully supports POSIX sockets; Windows supported with minor Winsock adapter.

- **Robust testing & documentation pipeline**  
  Unit tests via CTest + docs generation via CMake targets.

---

## 📦 Requirements

- **CMake ≥ 3.15**
- **C++23 compiler**
    - GCC ≥ 11
    - Clang ≥ 15
    - MSVC ≥ 19.3 (Visual Studio 2022+)
- **Platform**
    - Linux/macOS: out-of-the-box
    - Windows: requires Winsock mapping layer

---

## 🔧 Build Instructions

```bash
# Clone repository
git clone https://github.com/open-threshold-pqc/otpqcom.git
cd otpqcom

# Configure build
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build library + tests
cmake --build build --parallel

# Run test suite
ctest --test-dir build --output-on-failure
```

# 📘 Documentation

Generate documentation directly via CMake:

```bash
  cmake --build build --target docs
```
Output will be available in: `build/docs`


# 🧩 Usage / Project Integration

Add OTPQCOM as a git submodule:

```bash
  git submodule add https://github.com/youruser/otpqcom external/otpqcom
```


Add it to your project:

```
add_subdirectory(external/otpqcom)
target_link_libraries(your_target PRIVATE otpqcom)
```

# 📚 Citation

If you use OTPQCOM in academic or industrial research, please cite:

```
@software{otpqcom2024,
author       = {Kiarash Sedghi},
title        = {OTPQCOM: A Modern C++23 Multi-Peer Communication Library},
year         = {2024},
url          = {https://github.com/open-threshold-pqc/otpqcom.git},
note         = {Lightweight communication framework for distributed cryptographic protocols}
}
```


