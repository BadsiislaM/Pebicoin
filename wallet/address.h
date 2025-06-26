#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>
#include <vector>

namespace Pebicoin {

// Pebicoin version byte (prefix)
const unsigned char PEBICOIN_ADDRESS_VERSION = 0x37;

// Pebicoin address prefix string
const std::string PEBICOIN_ADDRESS_PREFIX = "Pbc";

// Generate a Pebicoin address from a public key hash
std::string generateAddressFromHash(const std::vector<unsigned char>& pubKeyHash);

// Generate a Pebicoin address from a public key
std::string generateAddressFromPublicKey(const std::vector<unsigned char>& publicKey);

// Decode a Base58Check address to get the version and hash
bool decodeAddress(const std::string& address, unsigned char& version, std::vector<unsigned char>& hash);

// Validate an address format and checksum
bool validateAddress(const std::string& address);

// Get the address prefix for Pebicoin addresses
std::string getAddressPrefix();

} // namespace Pebicoin

#endif // ADDRESS_H