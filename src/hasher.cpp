#include "hasher.h"
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace Pebicoin {

std::string doubleSHA256(const std::string& input) {
    unsigned char hash1[EVP_MAX_MD_SIZE];
    unsigned int hash1_len = 0;

    unsigned char hash2[EVP_MAX_MD_SIZE];
    unsigned int hash2_len = 0;

    // أول SHA256
    EVP_MD_CTX* ctx1 = EVP_MD_CTX_new();
    if (!ctx1) throw std::runtime_error("Failed to create EVP_MD_CTX for first hash");
    
    if (EVP_DigestInit_ex(ctx1, EVP_sha256(), nullptr) != 1 ||
        EVP_DigestUpdate(ctx1, input.data(), input.size()) != 1 ||
        EVP_DigestFinal_ex(ctx1, hash1, &hash1_len) != 1) {
        EVP_MD_CTX_free(ctx1);
        throw std::runtime_error("SHA256 hashing failed in first round");
    }
    EVP_MD_CTX_free(ctx1);

    // ثاني SHA256
    EVP_MD_CTX* ctx2 = EVP_MD_CTX_new();
    if (!ctx2) throw std::runtime_error("Failed to create EVP_MD_CTX for second hash");

    if (EVP_DigestInit_ex(ctx2, EVP_sha256(), nullptr) != 1 ||
        EVP_DigestUpdate(ctx2, hash1, hash1_len) != 1 ||
        EVP_DigestFinal_ex(ctx2, hash2, &hash2_len) != 1) {
        EVP_MD_CTX_free(ctx2);
        throw std::runtime_error("SHA256 hashing failed in second round");
    }
    EVP_MD_CTX_free(ctx2);

    std::stringstream ss;
    for (unsigned int i = 0; i < hash2_len; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash2[i]);
    }

    return ss.str();
}

} // namespace Pebicoin
