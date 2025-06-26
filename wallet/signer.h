#ifndef SIGNER_H
#define SIGNER_H

#include <string>

namespace Pebicoin {

// Sign a message using ECDSA with the Secp256k1 curve
std::string signMessage(const std::string& privateKey, const std::string& message);

// Verify a signature using ECDSA with the Secp256k1 curve
bool verifySignature(const std::string& publicKey, const std::string& message, const std::string& signature);

} // namespace Pebicoin

#endif // SIGNER_H