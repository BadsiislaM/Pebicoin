#ifndef WALLET_H
#define WALLET_H

#include <string>
#include <vector>
#include <map>
#include "../src/config.h"
#include "keys.h"
#include "transaction.h"

namespace Pebicoin {

class Wallet {
private:
    std::string walletFile;
    std::map<std::string, KeyPair> keyPairs; // Address -> KeyPair mapping
    Mizutsi balance;
    
    bool saveToFile();
    bool loadFromFile();
    
public:
    Wallet(const std::string& filename = "wallet.dat");
    ~Wallet();
    
    // Key management
    std::string generateNewAddress();
    std::vector<std::string> getAddresses() const;
    bool hasPrivateKey(const std::string& address) const;
    
    // Balance management
    Mizutsi getBalance() const;
    void updateBalance(const std::vector<Transaction>& transactions);
    
    // Transaction operations
    Transaction createTransaction(const std::string& fromAddress, const std::string& toAddress, Mizutsi amount, Mizutsi fee);
    bool signTransaction(Transaction& transaction, const std::string& fromAddress);
};

} // namespace Pebicoin

#endif // WALLET_H