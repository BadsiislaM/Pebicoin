#ifndef WALLET_HASHER_H
#define WALLET_HASHER_H

#include <string>
#include <vector>

namespace Pebicoin {

// Convert bytes to hex string
std::string bytesToHex(const unsigned char* data, size_t len);

// Convert hex string to bytes
std::vector<unsigned char> hexToBytes(const std::string& hex);

// Perform SHA-256 hash on input data
std::vector<unsigned char> sha256(const std::vector<unsigned char>& data);

// Perform double SHA-256 hash on input data
std::vector<unsigned char> doubleSha256(const std::vector<unsigned char>& data);

// Perform double SHA-256 hash on input string
std::string doubleSHA256(const std::string& data);

// Perform RIPEMD-160 hash on input data
std::vector<unsigned char> ripemd160(const std::vector<unsigned char>& data);

// Perform SHA-256 followed by RIPEMD-160 (used for public key to address hash)
std::vector<unsigned char> hash160(const std::vector<unsigned char>& data);

} // namespace Pebicoin

#endif // WALLET_HASHER_H