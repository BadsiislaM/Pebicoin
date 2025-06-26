// Modern OpenSSL 3.0+ key generation and public key derivation using EVP API
// Replaces deprecated EC_KEY_* API

#include "keys.h"
#include "crypto_utils.h"
#include "address.h"
#include "base58.h"
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <sstream>
#include <iomanip>
#include <vector>
#include <stdexcept>

namespace Pebicoin {

// Helper to convert EVP_PKEY to raw public key bytes
std::vector<unsigned char> extractPublicKey(EVP_PKEY* pkey, KeyFormat format) {
    size_t len = 0;
    std::vector<unsigned char> pubKey;

    if (EVP_PKEY_get_octet_string_param(pkey, OSSL_PKEY_PARAM_PUB_KEY, NULL, 0, &len) == 0 || len == 0)
        throw std::runtime_error("Failed to get public key length");

    pubKey.resize(len);
    if (EVP_PKEY_get_octet_string_param(pkey, OSSL_PKEY_PARAM_PUB_KEY, pubKey.data(), len, NULL) == 0)
        throw std::runtime_error("Failed to extract public key");

    return pubKey;
}

KeyPair generateKeyPair(KeyFormat format) {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
    if (!ctx)
        throw std::runtime_error("Failed to create EVP_PKEY_CTX");

    if (EVP_PKEY_keygen_init(ctx) <= 0)
        throw std::runtime_error("Failed to init keygen");

    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_secp256k1) <= 0)
        throw std::runtime_error("Failed to set curve");

    EVP_PKEY* pkey = nullptr;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0)
        throw std::runtime_error("Key generation failed");

    EVP_PKEY_CTX_free(ctx);

    // Get private key as raw octets
    size_t priv_len = 0;
    if (!EVP_PKEY_get_octet_string_param(pkey, OSSL_PKEY_PARAM_PRIV_KEY, NULL, 0, &priv_len))
        throw std::runtime_error("Failed to get private key length");

    std::vector<unsigned char> privKeyBin(priv_len);
    if (!EVP_PKEY_get_octet_string_param(pkey, OSSL_PKEY_PARAM_PRIV_KEY, privKeyBin.data(), priv_len, NULL))
        throw std::runtime_error("Failed to get private key");

    std::string privateKeyHex = bytesToHex(privKeyBin.data(), privKeyBin.size());

    std::vector<unsigned char> pubKeyBin = extractPublicKey(pkey, format);
    std::string publicKeyHex = bytesToHex(pubKeyBin.data(), pubKeyBin.size());

    KeyPair kp;
    kp.privateKey = privateKeyHex;
    kp.publicKey = publicKeyHex;
    kp.address = generateAddress(pubKeyBin);

    EVP_PKEY_free(pkey);
    return kp;
}

std::string getPublicKey(const std::string& privateKeyHex, KeyFormat format) {
    std::vector<unsigned char> privKey = hexToBytes(privateKeyHex);

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_from_name(NULL, "EC", NULL);
    if (!ctx)
        throw std::runtime_error("Failed to create context for EC");

    OSSL_PARAM params[] = {
        OSSL_PARAM_construct_octet_string(OSSL_PKEY_PARAM_PRIV_KEY, privKey.data(), privKey.size()),
        OSSL_PARAM_construct_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME, (char*)"secp256k1", 0),
        OSSL_PARAM_construct_end()
    };

    EVP_PKEY* pkey = EVP_PKEY_new_raw_private_key_ex(NULL, "EC", NULL, privKey.data(), privKey.size());
    if (!pkey)
        throw std::runtime_error("Failed to create raw private key");

    std::vector<unsigned char> pubKey = extractPublicKey(pkey, format);
    EVP_PKEY_free(pkey);

    return bytesToHex(pubKey.data(), pubKey.size());
}

} // namespace Pebicoin
