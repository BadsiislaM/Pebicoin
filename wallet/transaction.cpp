#include "transaction.h"
#include "../src/hasher.h"
#include "signer.h"
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace Pebicoin {

Transaction::Transaction() : timestamp(time(0)) {
    txid = calculateTxid();
}

Transaction::Transaction(const std::vector<TransactionInput>& inputs, 
                         const std::vector<TransactionOutput>& outputs)
    : inputs(inputs), outputs(outputs), timestamp(time(0)) {
    txid = calculateTxid();
}

std::string Transaction::calculateTxid() const {
    std::stringstream ss;
    
    // Add timestamp
    ss << timestamp;
    
    // Add all inputs
    for (const auto& input : inputs) {
        ss << input.txid << input.outputIndex;
    }
    
    // Add all outputs
    for (const auto& output : outputs) {
        ss << output.address << output.amount;
    }
    
    // Double SHA-256 hash
    return doubleSHA256(ss.str());
}

bool Transaction::sign(const std::string& privateKey, const std::string& publicKey) {
    // Create a message to sign (transaction ID)
    std::string message = calculateTxid();
    
    // Sign each input
    for (auto& input : inputs) {
        input.signature = signMessage(privateKey, message);
    }
    
    return true;
}

bool Transaction::verify() const {
    // TODO: لاحقًا - ابحث عن المعاملة السابقة لكل إدخال للحصول على المفتاح العام
    throw std::runtime_error("Transaction::verify not implemented. Requires previous output lookup.");
}

Mizutsi Transaction::getTotalInputAmount() const {
    // TODO: لاحقًا - اجمع قيمة كل output مشار إليه من الإدخالات
    throw std::runtime_error("Transaction::getTotalInputAmount not implemented. Requires previous output lookup.");
}

Mizutsi Transaction::getTotalOutputAmount() const {
    Mizutsi total = 0;
    for (const auto& output : outputs) {
        total += output.amount;
    }
    return total;
}

Mizutsi Transaction::getFee() const {
    return getTotalInputAmount() - getTotalOutputAmount();
}

} // namespace Pebicoin
