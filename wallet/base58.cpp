#include "base58.h"
#include <openssl/evp.h>
#include <algorithm>
#include <stdexcept>

namespace Pebicoin {

const char* BASE58_CHARS = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

std::string base58Encode(const std::vector<unsigned char>& data) {
    size_t zeros = 0;
    while (zeros < data.size() && data[zeros] == 0) {
        zeros++;
    }

    std::vector<unsigned char> b58(data.size() * 138 / 100 + 1);

    for (size_t i = zeros; i < data.size(); i++) {
        int carry = data[i];
        for (int j = b58.size() - 1; j >= 0; j--) {
            carry += 256 * b58[j];
            b58[j] = carry % 58;
            carry /= 58;
        }
    }

    size_t i = 0;
    while (i < b58.size() && b58[i] == 0) {
        i++;
    }

    std::string result;
    result.assign(zeros, '1');
    while (i < b58.size()) {
        result += BASE58_CHARS[b58[i]];
        i++;
    }

    return result;
}

bool base58Decode(const std::string& encoded, std::vector<unsigned char>& result) {
    result.clear();
    result.reserve(encoded.size());

    for (char c : encoded) {
        const char* p = std::find(BASE58_CHARS, BASE58_CHARS + 58, c);
        if (p == BASE58_CHARS + 58) return false;

        int carry = p - BASE58_CHARS;
        for (int i = result.size() - 1; i >= 0; i--) {
            carry += 58 * result[i];
            result[i] = carry % 256;
            carry /= 256;
        }

        if (carry > 0) {
            result.insert(result.begin(), carry);
        }
    }

    for (size_t i = 0; i < encoded.size() && encoded[i] == '1'; i++) {
        result.insert(result.begin(), static_cast<unsigned char>(0));
    }

    return true;
}

// ✅ دالة حديثة لحساب SHA256
std::vector<unsigned char> sha256Evp(const std::vector<unsigned char>& data) {
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

std::string base58CheckEncode(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> checksum1 = sha256Evp(data);
    std::vector<unsigned char> checksum2 = sha256Evp(checksum1);

    std::vector<unsigned char> dataWithChecksum = data;
    dataWithChecksum.insert(dataWithChecksum.end(), checksum2.begin(), checksum2.begin() + 4);

    return base58Encode(dataWithChecksum);
}

bool base58CheckDecode(const std::string& encoded, std::vector<unsigned char>& result) {
    std::vector<unsigned char> decoded;
    if (!base58Decode(encoded, decoded)) {
        return false;
    }

    if (decoded.size() < 4) return false;

    std::vector<unsigned char> data(decoded.begin(), decoded.end() - 4);
    std::vector<unsigned char> checksumFromData(decoded.end() - 4, decoded.end());

    std::vector<unsigned char> checksum1 = sha256Evp(data);
    std::vector<unsigned char> checksum2 = sha256Evp(checksum1);

    for (int i = 0; i < 4; i++) {
        if (checksum2[i] != checksumFromData[i]) {
            return false;
        }
    }

    result = data;
    return true;
}

} // namespace Pebicoin
