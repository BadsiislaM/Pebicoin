#ifndef KEYS_H
#define KEYS_H

#include <string>
#include <vector>
#include <openssl/ec.h>

namespace Pebicoin {

// Key formats
enum class KeyFormat {
    COMPRESSED,    // Compressed public key format (33 bytes)
    UNCOMPRESSED   // Uncompressed public key format (65 bytes)
};

struct KeyPair {
    std::string privateKey;  // Hex-encoded private key
    std::string publicKey;   // Hex-encoded public key
    std::string address;     // Pebicoin address
};

// Generate a new key pair using Secp256k1 curve
KeyPair generateKeyPair(KeyFormat format = KeyFormat::COMPRESSED);

// Get public key from private key
std::string getPublicKey(const std::string& privateKey, KeyFormat format = KeyFormat::COMPRESSED);

// Convert public key string to bytes
std::vector<unsigned char> publicKeyToBytes(const std::string& publicKey);

// Generate an address from a public key using SHA256 -> RIPEMD160 -> Base58Check
std::string generateAddress(const std::string& publicKey);

// Generate an address from a public key in bytes
std::string generateAddress(const std::vector<unsigned char>& publicKeyBytes);

// Verify if an address is valid
bool isValidAddress(const std::string& address);

} // namespace Pebicoin

#endif // KEYS_H