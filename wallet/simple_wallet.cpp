#include <iostream>
#include <string>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/bn.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>

// Function to convert bytes to hex string
std::string bytesToHex(const unsigned char* data, size_t len) {
    std::string result;
    result.reserve(len * 2);
    for (size_t i = 0; i < len; ++i) {
        char hex[3];
        sprintf(hex, "%02x", data[i]);
        result += hex;
    }
    return result;
}

// Function to generate a key pair
void generateKeyPair(std::string& privateKey, std::string& publicKey) {
    EC_KEY* ecKey = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!ecKey) {
        std::cerr << "Failed to create EC key" << std::endl;
        return;
    }

    if (EC_KEY_generate_key(ecKey) != 1) {
        std::cerr << "Failed to generate EC key" << std::endl;
        EC_KEY_free(ecKey);
        return;
    }

    // Get private key
    const BIGNUM* privKey = EC_KEY_get0_private_key(ecKey);
    if (privKey) {
        char* hexPrivKey = BN_bn2hex(privKey);
        privateKey = std::string(hexPrivKey);
        OPENSSL_free(hexPrivKey);
    }

    // Get public key
    const EC_POINT* pubKey = EC_KEY_get0_public_key(ecKey);
    if (pubKey) {
        EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_secp256k1);
        if (group) {
            point_conversion_form_t form = POINT_CONVERSION_COMPRESSED;
            char* hexPubKey = EC_POINT_point2hex(group, pubKey, form, NULL);
            publicKey = std::string(hexPubKey);
            OPENSSL_free(hexPubKey);
            EC_GROUP_free(group);
        }
    }

    EC_KEY_free(ecKey);
}

// Function to create a Pebicoin address from a public key
std::string createAddress(const std::string& publicKeyHex) {
    // Convert hex public key to binary
    size_t len = publicKeyHex.length() / 2;
    unsigned char* pubKey = new unsigned char[len];
    for (size_t i = 0; i < len; i++) {
        sscanf(publicKeyHex.c_str() + 2 * i, "%2hhx", &pubKey[i]);
    }

    // SHA-256 hash
    unsigned char sha256Hash[SHA256_DIGEST_LENGTH];
    SHA256(pubKey, len, sha256Hash);

    // RIPEMD-160 hash
    unsigned char ripemd160Hash[RIPEMD160_DIGEST_LENGTH];
    RIPEMD160(sha256Hash, SHA256_DIGEST_LENGTH, ripemd160Hash);

    // Add version byte (0x00 for Pebicoin)
    unsigned char versionedHash[RIPEMD160_DIGEST_LENGTH + 1];
    versionedHash[0] = 0x00;
    memcpy(versionedHash + 1, ripemd160Hash, RIPEMD160_DIGEST_LENGTH);

    // Double SHA-256 for checksum
    unsigned char checksum[SHA256_DIGEST_LENGTH];
    SHA256(versionedHash, RIPEMD160_DIGEST_LENGTH + 1, sha256Hash);
    SHA256(sha256Hash, SHA256_DIGEST_LENGTH, checksum);

    // Combine versioned hash and checksum
    unsigned char addressBytes[RIPEMD160_DIGEST_LENGTH + 1 + 4];
    memcpy(addressBytes, versionedHash, RIPEMD160_DIGEST_LENGTH + 1);
    memcpy(addressBytes + RIPEMD160_DIGEST_LENGTH + 1, checksum, 4);

    // Convert to Base58 (simplified - just hex for now)
    std::string address = bytesToHex(addressBytes, RIPEMD160_DIGEST_LENGTH + 1 + 4);

    delete[] pubKey;
    return address;
}

int main() {
    std::string privateKey, publicKey;
    generateKeyPair(privateKey, publicKey);

    std::string address = createAddress(publicKey);

    std::cout << "Pebicoin Wallet - Address Generator" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "Private Key: " << privateKey << std::endl;
    std::cout << "Public Key: " << publicKey << std::endl;
    std::cout << "Address: " << address << std::endl;
    std::cout << "\nWARNING: Keep your private key secure!" << std::endl;
    std::cout << "Never share your private key with anyone!" << std::endl;

    return 0;
}