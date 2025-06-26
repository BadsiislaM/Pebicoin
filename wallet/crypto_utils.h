#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <string>
#include <vector>

namespace Pebicoin {

std::vector<unsigned char> hexToBytes(const std::string& hex);
std::string bytesToHex(const unsigned char* data, size_t length);

std::vector<unsigned char> sha256(const std::vector<unsigned char>& data);
std::vector<unsigned char> ripemd160(const std::vector<unsigned char>& data);
std::vector<unsigned char> doubleSha256(const std::vector<unsigned char>& data);

// ✅ تصريح جديد لـ hash160
std::vector<unsigned char> hash160(const std::vector<unsigned char>& data);

} // namespace Pebicoin

#endif // CRYPTO_UTILS_H
