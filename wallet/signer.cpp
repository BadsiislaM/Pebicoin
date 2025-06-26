#include "signer.h"
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include "crypto_utils.h"

namespace Pebicoin {

std::string signMessage(const std::string& privateKeyHex, const std::string& message) {
    std::vector<unsigned char> privKey = hexToBytes(privateKeyHex);

    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    EVP_PKEY* pkey = nullptr;
    if (!pctx) throw std::runtime_error("Failed to create PKEY context");

    if (EVP_PKEY_fromdata_init(pctx) <= 0)
        throw std::runtime_error("Failed to init fromdata");

    OSSL_PARAM params[] = {
        OSSL_PARAM_construct_octet_string("priv", privKey.data(), privKey.size()),
        OSSL_PARAM_construct_utf8_string("group", (char*)"secp256k1", 0),
        OSSL_PARAM_construct_end()
    };

    if (EVP_PKEY_fromdata(pctx, &pkey, EVP_PKEY_KEYPAIR, params) <= 0)
        throw std::runtime_error("Failed to create PKEY from data");

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(message.c_str()), message.length(), hash);

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) throw std::runtime_error("Failed to create MD context");

    if (EVP_DigestSignInit(mdctx, nullptr, EVP_sha256(), nullptr, pkey) <= 0)
        throw std::runtime_error("Failed to initialize DigestSign");

    size_t sigLen = 0;
    if (EVP_DigestSign(mdctx, nullptr, &sigLen, hash, sizeof(hash)) <= 0)
        throw std::runtime_error("Failed to get signature length");

    std::vector<unsigned char> signature(sigLen);
    if (EVP_DigestSign(mdctx, signature.data(), &sigLen, hash, sizeof(hash)) <= 0)
        throw std::runtime_error("Failed to generate signature");

    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(pctx);

    return bytesToHex(signature.data(), sigLen);
}

bool verifySignature(const std::string& publicKeyHex, const std::string& message, const std::string& signatureHex) {
    std::vector<unsigned char> pubKey = hexToBytes(publicKeyHex);
    std::vector<unsigned char> sigBytes = hexToBytes(signatureHex);

    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    EVP_PKEY* pkey = nullptr;
    if (!pctx) throw std::runtime_error("Failed to create PKEY context");

    if (EVP_PKEY_fromdata_init(pctx) <= 0)
        throw std::runtime_error("Failed to init fromdata");

    OSSL_PARAM params[] = {
        OSSL_PARAM_construct_octet_string("pub", pubKey.data(), pubKey.size()),
        OSSL_PARAM_construct_utf8_string("group", (char*)"secp256k1", 0),
        OSSL_PARAM_construct_end()
    };

    if (EVP_PKEY_fromdata(pctx, &pkey, EVP_PKEY_PUBLIC_KEY, params) <= 0)
        throw std::runtime_error("Failed to create public key");

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(message.c_str()), message.length(), hash);

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) throw std::runtime_error("Failed to create MD context");

    if (EVP_DigestVerifyInit(mdctx, nullptr, EVP_sha256(), nullptr, pkey) <= 0)
        throw std::runtime_error("Failed to initialize DigestVerify");

    int result = EVP_DigestVerify(mdctx, sigBytes.data(), sigBytes.size(), hash, sizeof(hash));

    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(pctx);

    return result == 1;
}

} // namespace Pebicoin
