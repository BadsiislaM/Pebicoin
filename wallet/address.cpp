#include "address.h"
#include "hasher.h"
#include "base58.h"
#include "crypto_utils.h" // ✅ ضروري للوصول إلى hexToBytes و hash160
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>

namespace Pebicoin {

// Generate a Pebicoin address from a public key hash
std::string generateAddressFromHash(const std::vector<unsigned char>& pubKeyHash) {
    std::vector<unsigned char> versionedHash;
    versionedHash.push_back(PEBICOIN_ADDRESS_VERSION);
    versionedHash.insert(versionedHash.end(), pubKeyHash.begin(), pubKeyHash.end());

    std::vector<unsigned char> checksum = doubleSha256(versionedHash);
    versionedHash.insert(versionedHash.end(), checksum.begin(), checksum.begin() + 4);

    std::string address = PEBICOIN_ADDRESS_PREFIX + base58Encode(versionedHash);
    return address;
}

// Generate a Pebicoin address from a public key
std::string generateAddressFromPublicKey(const std::vector<unsigned char>& publicKey) {
    std::vector<unsigned char> pubKeyHash = hash160(publicKey);
    return generateAddressFromHash(pubKeyHash);
}

// ✅ إضافة النسختين المطلوبتين من generateAddress

// من سلسلة hex
std::string generateAddress(const std::string& publicKeyHex) {
    std::vector<unsigned char> pubKeyBytes = hexToBytes(publicKeyHex);
    return generateAddressFromPublicKey(pubKeyBytes);
}

// من bytes مباشرة
std::string generateAddress(const std::vector<unsigned char>& publicKey) {
    return generateAddressFromPublicKey(publicKey);
}

bool decodeAddress(const std::string& address, unsigned char& version, std::vector<unsigned char>& hash) {
    if (address.substr(0, PEBICOIN_ADDRESS_PREFIX.length()) != PEBICOIN_ADDRESS_PREFIX) {
        return false;
    }

    std::string base58Part = address.substr(PEBICOIN_ADDRESS_PREFIX.length());
    std::vector<unsigned char> decoded;
    if (!base58Decode(base58Part, decoded)) {
        return false;
    }

    if (decoded.size() < 5) {
        return false;
    }

    version = decoded[0];
    hash.assign(decoded.begin() + 1, decoded.end() - 4);

    std::vector<unsigned char> checkData(decoded.begin(), decoded.end() - 4);
    std::vector<unsigned char> checksum = doubleSha256(checkData);

    for (int i = 0; i < 4; i++) {
        if (checksum[i] != decoded[decoded.size() - 4 + i]) {
            return false;
        }
    }

    return true;
}

bool validateAddress(const std::string& address) {
    if (address.substr(0, PEBICOIN_ADDRESS_PREFIX.length()) != PEBICOIN_ADDRESS_PREFIX) {
        return false;
    }

    unsigned char version;
    std::vector<unsigned char> hash;
    return decodeAddress(address, version, hash);
}

std::string getAddressPrefix() {
    return PEBICOIN_ADDRESS_PREFIX;
}

} // namespace Pebicoin
