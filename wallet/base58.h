#ifndef BASE58_H
#define BASE58_H

#include <string>
#include <vector>

namespace Pebicoin {

// Base58 encoding alphabet
extern const char* BASE58_CHARS;

// Encode binary data to Base58
std::string base58Encode(const std::vector<unsigned char>& data);

// Decode Base58 string to binary data
bool base58Decode(const std::string& encoded, std::vector<unsigned char>& result);

// Base58Check encoding (with checksum)
std::string base58CheckEncode(const std::vector<unsigned char>& data);

// Base58Check decoding (with checksum verification)
bool base58CheckDecode(const std::string& encoded, std::vector<unsigned char>& result);

} // namespace Pebicoin

#endif // BASE58_H