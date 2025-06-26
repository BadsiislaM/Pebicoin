#include "blockchain.h"
#include "config.h"
#include "block.h"
#include <iostream>
#include <ctime>

namespace Pebicoin {

Blockchain::Blockchain() : difficulty(STARTING_DIFFICULTY) {
    // Create the genesis block with the specified date
    struct tm genesisDate = {0};
    genesisDate.tm_year = 2025 - 1900; // Years since 1900
    genesisDate.tm_mon = 6 - 1;       // Month (0-11)
    genesisDate.tm_mday = 21;          // Day of month
    genesisDate.tm_hour = 12;          // Noon
    time_t genesisTime = mktime(&genesisDate);
    
    Block genesisBlock(0, genesisTime, "Genesis Block - Pebicoin", 0, "0");
    genesisBlock.hash = genesisBlock.calculateHash();
    chain.push_back(genesisBlock);
    
    lastDifficultyAdjustmentTime = genesisTime;
}

// This method is replaced by the more comprehensive implementation below
// void Blockchain::addBlock(Block newBlock) {
//     newBlock.previousHash = getLastBlock().hash;
//     newBlock.mineBlock(difficulty);
//     chain.push_back(newBlock);
// }

// These methods are replaced by the more comprehensive implementations below
// bool Blockchain::isChainValid() const {
//     for (size_t i = 1; i < chain.size(); i++) {
//         const Block& currentBlock = chain[i];
//         const Block& previousBlock = chain[i - 1];
//
//         if (currentBlock.hash != currentBlock.calculateHash()) {
//             return false;
//         }
//
//         if (currentBlock.previousHash != previousBlock.hash) {
//             return false;
//         }
//     }
//     return true;
// }
//
// Block Blockchain::getLastBlock() const {
//     return chain.back();
// }

void Blockchain::addBlock(Block newBlock) {
    // Verify the block's previous hash matches the last block's hash
    if (newBlock.previousHash != getLastBlock().hash) {
        std::cerr << "Invalid block: previous hash doesn't match" << std::endl;
        return;
    }
    
    // Verify the block's hash meets the difficulty requirement
    std::string target(difficulty, '0');
    if (newBlock.hash.substr(0, difficulty) != target) {
        std::cerr << "Invalid block: hash doesn't meet difficulty target" << std::endl;
        return;
    }
    
    // Add the block to the chain
    chain.push_back(newBlock);
    
    // Adjust difficulty every 2016 blocks (approximately 2 weeks with 10-minute blocks)
    if (chain.size() % 2016 == 0) {
        adjustDifficulty();
    }
}

bool Blockchain::isChainValid() const {
    for (size_t i = 1; i < chain.size(); i++) {
        const Block& currentBlock = chain[i];
        const Block& previousBlock = chain[i - 1];
        
        // Check if the hash is correctly calculated
        if (currentBlock.hash != currentBlock.calculateHash()) {
            return false;
        }
        
        // Check if the previous hash reference is correct
        if (currentBlock.previousHash != previousBlock.hash) {
            return false;
        }
        
        // Check if the hash meets the difficulty requirement
        std::string target(difficulty, '0');
        if (currentBlock.hash.substr(0, difficulty) != target) {
            return false;
        }
    }
    
    return true;
}

Block Blockchain::getLastBlock() const {
    return chain.back();
}

void Blockchain::adjustDifficulty() {
    // Get the time of the last block
    time_t currentTime = getLastBlock().timestamp;
    
    // Calculate the time taken for the last 2016 blocks
    time_t timeExpected = 2016 * BLOCK_TIME; // Expected time: 2016 blocks * 10 minutes
    time_t timeTaken = currentTime - lastDifficultyAdjustmentTime;
    
    // Adjust difficulty based on how long it took to mine the last 2016 blocks
    if (timeTaken < timeExpected / 4) {
        // Blocks are being mined too quickly, increase difficulty
        difficulty++;
    } else if (timeTaken > timeExpected * 4) {
        // Blocks are being mined too slowly, decrease difficulty (but not below 1)
        if (difficulty > 1) {
            difficulty--;
        }
    }
    
    // Update the last adjustment time
    lastDifficultyAdjustmentTime = currentTime;
    
    std::cout << "Difficulty adjusted to: " << difficulty << std::endl;
}

} // namespace Pebicoin
