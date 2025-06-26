#include "wallet.h"
#include "keys.h"
#include "signer.h"
#include <fstream>
#include <iostream>
#include "nlohmann/json.hpp"

namespace Pebicoin {

Wallet::Wallet(const std::string& filename) : walletFile(filename), balance(0) {
    // Try to load existing wallet file
    if (!loadFromFile()) {
        std::cout << "Creating new wallet file: " << filename << std::endl;
    }
}

Wallet::~Wallet() {
    // Save wallet data when destroyed
    saveToFile();
}

bool Wallet::saveToFile() {
    try {
        nlohmann::json walletJson;
        
        // Save addresses and private keys
        nlohmann::json keysJson = nlohmann::json::array();
        for (const auto& pair : keyPairs) {
            nlohmann::json keyJson;
            keyJson["address"] = pair.first;
            keyJson["private_key"] = pair.second.privateKey;
            keyJson["public_key"] = pair.second.publicKey;
            keysJson.push_back(keyJson);
        }
        
        walletJson["keys"] = keysJson;
        
        // Write to file
        std::ofstream file(walletFile);
        if (!file.is_open()) {
            std::cerr << "Failed to open wallet file for writing: " << walletFile << std::endl;
            return false;
        }
        
        file << walletJson.dump(4); // Pretty print with 4-space indent
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving wallet: " << e.what() << std::endl;
        return false;
    }
}

bool Wallet::loadFromFile() {
    try {
        std::ifstream file(walletFile);
        if (!file.is_open()) {
            return false; // File doesn't exist yet
        }
        
        nlohmann::json walletJson;
        file >> walletJson;
        
        // Load keys
        if (walletJson.contains("keys") && walletJson["keys"].is_array()) {
            for (const auto& keyJson : walletJson["keys"]) {
                std::string address = keyJson["address"];
                KeyPair keyPair;
                keyPair.privateKey = keyJson["private_key"];
                keyPair.publicKey = keyJson["public_key"];
                keyPairs[address] = keyPair;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading wallet: " << e.what() << std::endl;
        return false;
    }
}

std::string Wallet::generateNewAddress() {
    // Generate a new key pair
    KeyPair keyPair = generateKeyPair();
    
    // Generate address from public key
    std::string address = generateAddress(keyPair.publicKey);
    
    // Store in wallet
    keyPairs[address] = keyPair;
    
    // Save wallet
    saveToFile();
    
    return address;
}

std::vector<std::string> Wallet::getAddresses() const {
    std::vector<std::string> addresses;
    for (const auto& pair : keyPairs) {
        addresses.push_back(pair.first);
    }
    return addresses;
}

bool Wallet::hasPrivateKey(const std::string& address) const {
    return keyPairs.find(address) != keyPairs.end();
}

Mizutsi Wallet::getBalance() const {
    return balance;
}

void Wallet::updateBalance(const std::vector<Transaction>& transactions) {
    // Reset balance
    balance = 0;
    
    // Get all addresses in this wallet
    std::vector<std::string> addresses = getAddresses();
    
    // Process each transaction
    for (const auto& tx : transactions) {
        // Check outputs for payments to our addresses
        for (const auto& output : tx.outputs) {
            // If this output is to one of our addresses, add to balance
            if (std::find(addresses.begin(), addresses.end(), output.address) != addresses.end()) {
                balance += output.amount;
            }
        }
        
        // Check inputs for payments from our addresses
        for (const auto& input : tx.inputs) {
            // TODO: Check if this input spends from one of our addresses
            // This requires access to the blockchain to look up previous transactions
            // For now, we'll just assume it doesn't affect our balance
        }
    }
}

Transaction Wallet::createTransaction(const std::string& fromAddress, const std::string& toAddress, 
                                     Mizutsi amount, Mizutsi fee) {
    // Check if we have the private key for the from address
    if (!hasPrivateKey(fromAddress)) {
        throw std::runtime_error("Cannot create transaction: private key not found for address");
    }
    
    // Check if we have enough balance
    if (balance < amount + fee) {
        throw std::runtime_error("Insufficient balance for transaction");
    }
    
    // Create a new transaction
    Transaction tx;
    
    // TODO: Add proper inputs from unspent transaction outputs
    // For now, we'll create a placeholder input
    TransactionInput input;
    input.txid = "0000000000000000000000000000000000000000000000000000000000000000";
    input.outputIndex = 0;
    tx.inputs.push_back(input);
    
    // Add the output to the recipient
    TransactionOutput output;
    output.address = toAddress;
    output.amount = amount;
    tx.outputs.push_back(output);
    
    // Add change output if needed
    if (balance > amount + fee) {
        TransactionOutput changeOutput;
        changeOutput.address = fromAddress;
        changeOutput.amount = balance - amount - fee;
        tx.outputs.push_back(changeOutput);
    }
    
    return tx;
}

bool Wallet::signTransaction(Transaction& transaction, const std::string& fromAddress) {
    // Check if we have the private key for the from address
    if (!hasPrivateKey(fromAddress)) {
        return false;
    }
    
    // Get the key pair
    const KeyPair& keyPair = keyPairs[fromAddress];
    
    // Sign the transaction
    return transaction.sign(keyPair.privateKey, keyPair.publicKey);
}

} // namespace Pebicoin
