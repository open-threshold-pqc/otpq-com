# OTPQCOM

OTPQCOM is a modern C++23 communication library designed for multi-peer networked applications.  
It provides abstractions for sockets, client/server communication, and peer-to-peer messaging.

---

## ✨ Features

- **C++23** clean and modern codebase
- **Configurable network I/O buffer size & mode**
- **Client and server socket channels** with RAII cleanup
- **Multi-peer communication layer (MPCommunication)**
- **Unit tests** integrated via CTest
- **Documentation** build with CMake

---

## 📦 Requirements

- **CMake ≥ 3.15**
- **C++23 compiler** (GCC ≥ 11, Clang ≥ 15, MSVC ≥ 19.3+)
- POSIX sockets (Linux/macOS; Windows requires Winsock adaptation)

---

## ⚙️ Build Instructions

```bash
# Clone repository
git clone https://github.com/youruser/otpqcom.git
cd otpqcom

# Configure build
cmake -B build

# Build library + tests
cmake --build build

# Run tests
ctest --test-dir build
