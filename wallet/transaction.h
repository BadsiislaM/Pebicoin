#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <vector>
#include <ctime>
#include "../src/config.h"

namespace Pebicoin {

struct TransactionInput {
    std::string txid;      // Reference to previous transaction
    uint32_t outputIndex;  // Index of the output in the previous transaction
    std::string signature; // Signature to unlock the output
};

struct TransactionOutput {
    std::string address;   // Recipient address
    Mizutsi amount;        // Amount in Mizutsi
};

class Transaction {
public:
    std::string txid;                      // Transaction ID (hash)
    time_t timestamp;                      // Transaction timestamp
    std::vector<TransactionInput> inputs;  // Transaction inputs
    std::vector<TransactionOutput> outputs; // Transaction outputs
    
    Transaction();
    Transaction(const std::vector<TransactionInput>& inputs, 
                const std::vector<TransactionOutput>& outputs);
    
    // Calculate the transaction ID (hash)
    std::string calculateTxid() const;
    
    // Sign the transaction with a private key
    bool sign(const std::string& privateKey, const std::string& publicKey);
    
    // Verify the transaction signature
    bool verify() const;
    
    // Get the total input amount
    Mizutsi getTotalInputAmount() const;
    
    // Get the total output amount
    Mizutsi getTotalOutputAmount() const;
    
    // Get the transaction fee
    Mizutsi getFee() const;
};

} // namespace Pebicoin

#endif // TRANSACTION_H