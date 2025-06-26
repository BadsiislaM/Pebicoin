# Pebicoin Wallet

A standalone wallet address generator for Pebicoin cryptocurrency.

## Features

- Generates Pebicoin private keys and addresses
- Uses secp256k1 for key generation and ECDSA
- Implements SHA-256 and RIPEMD-160 for address hashing
- Base58Check encoding with Pebicoin-specific prefix
- Statically linked for maximum portability
- No runtime dependencies

## Build Instructions

### Using CMake (Recommended)

```bash
# Create a build directory
mkdir -p build
cd build

# Configure with CMake
cmake ..

# Build the project
cmake --build . --config Release
```

The executables will be placed in the `PebicoinWallet` directory.

### Using MinGW Directly

For Windows, you can use the provided batch file:

```bash
# Run the build script
build_wallet.bat
```

For Linux/Unix systems with MinGW cross-compiler installed:

```bash
# Make the script executable
chmod +x build_wallet.sh

# Run the build script
./build_wallet.sh
```

## Manual Compilation

### 64-bit Windows Executable

```bash
x86_64-w64-mingw32-g++ main.cpp wallet.cpp keys.cpp address.cpp hasher.cpp base58.cpp signer.cpp transaction.cpp -o wallet_64.exe -lssl -lcrypto -static -std=c++17
```

### 32-bit Windows Executable

```bash
i686-w64-mingw32-g++ main.cpp wallet.cpp keys.cpp address.cpp hasher.cpp base58.cpp signer.cpp transaction.cpp -o wallet_32.exe -lssl -lcrypto -static -std=c++17
```

## Dependencies

- OpenSSL (statically linked)
- C++17 compatible compiler

## Project Structure

- `main.cpp` - CLI entry point
- `wallet.cpp/h` - Wallet generation logic
- `keys.cpp/h` - Private/public key generation (secp256k1)
- `address.cpp/h` - Address creation logic
- `hasher.cpp/h` - SHA-256 and RIPEMD-160
- `base58.cpp/h` - Base58Check encoding
- `signer.cpp/h` - ECDSA signing and verification
- `transaction.cpp/h` - Transaction structure and operations