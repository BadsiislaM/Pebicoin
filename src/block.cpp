#include "block.h"
#include "hasher.h"
#include <sstream>
#include <iostream>

namespace Pebicoin {

Block::Block(uint32_t index, time_t timestamp, std::string data, uint32_t nonce, std::string previousHash) :
    index(index), timestamp(timestamp), data(data), nonce(nonce), previousHash(previousHash) {
    hash = calculateHash();
}

std::string Block::calculateHash() const {
    std::stringstream ss;
    ss << index << timestamp << data << nonce << previousHash;
    return doubleSHA256(ss.str());
}

void Block::mineBlock(uint32_t difficulty) {
    std::cout << "Mining block with difficulty: " << difficulty << "..." << std::endl;
    
    // Create target string with leading zeros based on difficulty
    std::string target(difficulty, '0');
    
    // Track mining progress
    uint32_t hashesComputed = 0;
    const uint32_t reportInterval = 100000; // Report progress every 100k hashes
    
    // Mine until we find a hash with the required number of leading zeros
    while (hash.substr(0, difficulty) != target) {
        nonce++;
        hash = calculateHash();
        hashesComputed++;
        
        // Report progress periodically
        if (hashesComputed % reportInterval == 0) {
            std::cout << "Computed " << hashesComputed << " hashes, current nonce: " << nonce << std::endl;
        }
    }
    
    std::cout << "Block Mined! Hash: " << hash << std::endl;
    std::cout << "Nonce: " << nonce << " (" << hashesComputed << " hashes computed)" << std::endl;
}

} // namespace Pebicoin
