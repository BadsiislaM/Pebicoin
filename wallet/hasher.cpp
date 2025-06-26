#include "hasher.h"
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <sstream>
#include <iomanip>

namespace Pebicoin {

// Convert bytes to hex string
std::string bytesToHex(const unsigned char* data, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; i++) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    return ss.str();
}

// Convert hex string to bytes
std::vector<unsigned char> hexToBytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoi(byteString, nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

// Perform SHA-256 hash on input data
std::vector<unsigned char> sha256(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.data(), data.size());
    SHA256_Final(hash.data(), &sha256);
    return hash;
}

// Perform double SHA-256 hash on input data
std::vector<unsigned char> doubleSha256(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> hash = sha256(data);
    return sha256(hash);
}

// Perform double SHA-256 hash on input string
std::string doubleSHA256(const std::string& data) {
    // Convert string to bytes
    std::vector<unsigned char> dataBytes(data.begin(), data.end());
    
    // Perform double SHA-256
    std::vector<unsigned char> hash = doubleSha256(dataBytes);
    
    // Convert result to hex string
    return bytesToHex(hash.data(), hash.size());
}

// Perform RIPEMD-160 hash on input data
std::vector<unsigned char> ripemd160(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> hash(RIPEMD160_DIGEST_LENGTH);
    RIPEMD160_CTX ripemd;
    RIPEMD160_Init(&ripemd);
    RIPEMD160_Update(&ripemd, data.data(), data.size());
    RIPEMD160_Final(hash.data(), &ripemd);
    return hash;
}

// Perform SHA-256 followed by RIPEMD-160 (used for public key to address hash)
std::vector<unsigned char> hash160(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> sha256Hash = sha256(data);
    return ripemd160(sha256Hash);
}

} // namespace Pebicoin