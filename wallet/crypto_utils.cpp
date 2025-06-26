#include "crypto_utils.h"
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace Pebicoin {

std::vector<unsigned char> hexToBytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    if (hex.size() % 2 != 0) return bytes;

    for (size_t i = 0; i < hex.size(); i += 2) {
        unsigned int byte;
        std::stringstream ss;
        ss << std::hex << hex.substr(i, 2);
        ss >> byte;
        bytes.push_back(static_cast<unsigned char>(byte));
    }
    return bytes;
}

std::string bytesToHex(const unsigned char* data, size_t length) {
    std::ostringstream oss;
    for (size_t i = 0; i < length; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]);
    }
    return oss.str();
}

std::vector<unsigned char> sha256(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> hash(EVP_MAX_MD_SIZE);
    unsigned int hashLen = 0;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create SHA256 context");

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1 ||
        EVP_DigestUpdate(ctx, data.data(), data.size()) != 1 ||
        EVP_DigestFinal_ex(ctx, hash.data(), &hashLen) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("SHA256 hashing failed");
    }

    EVP_MD_CTX_free(ctx);
    hash.resize(hashLen);
    return hash;
}

std::vector<unsigned char> ripemd160(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> hash(EVP_MAX_MD_SIZE);
    unsigned int hashLen = 0;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) throw std::runtime_error("Failed to create RIPEMD160 context");

    if (EVP_DigestInit_ex(ctx, EVP_ripemd160(), nullptr) != 1 ||
        EVP_DigestUpdate(ctx, data.data(), data.size()) != 1 ||
        EVP_DigestFinal_ex(ctx, hash.data(), &hashLen) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("RIPEMD160 hashing failed");
    }

    EVP_MD_CTX_free(ctx);
    hash.resize(hashLen);
    return hash;
}

std::vector<unsigned char> doubleSha256(const std::vector<unsigned char>& data) {
    return sha256(sha256(data));
}

// NEW: hash160 = RIPEMD160(SHA256(data))
std::vector<unsigned char> hash160(const std::vector<unsigned char>& data) {
    return ripemd160(sha256(data));
}

} // namespace Pebicoin
