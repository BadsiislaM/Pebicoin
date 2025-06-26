#include "miner.h"
#include "config.h"
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

namespace Pebicoin {

void Miner::mineBlock(Blockchain& blockchain, std::string minerAddress) {
    std::cout << "Mining a new block..." << std::endl;
    
    // Get the last block
    Block lastBlock = blockchain.getLastBlock();
    
    // Create a new block with the previous hash
    uint32_t newIndex = lastBlock.index + 1;
    time_t timestamp = time(0);
    std::string data = "coinbase:" + minerAddress + ":" + std::to_string(getBlockReward(newIndex));
    Block newBlock(newIndex, timestamp, data, 0, lastBlock.hash);
    
    // Mine the block with the current difficulty
    newBlock.mineBlock(blockchain.difficulty);
    
    // Add the block to the blockchain
    blockchain.addBlock(newBlock);
    
    std::cout << "Block mined by: " << minerAddress << std::endl;
    std::cout << "Block hash: " << newBlock.hash << std::endl;
    std::cout << "Block reward: " << getBlockReward(newIndex) << " PBC" << std::endl;
}

void Miner::mineBlockMultiThreaded(Blockchain& blockchain, std::string minerAddress, uint32_t numThreads) {
    std::cout << "Mining a new block with " << numThreads << " threads..." << std::endl;
    
    // Get the last block
    Block lastBlock = blockchain.getLastBlock();
    
    // Create a new block with the previous hash
    uint32_t newIndex = lastBlock.index + 1;
    time_t timestamp = time(0);
    std::string data = "coinbase:" + minerAddress + ":" + std::to_string(getBlockReward(newIndex));
    
    // Shared variables for mining
    std::atomic<bool> blockFound(false);
    std::atomic<uint32_t> sharedNonce(0);
    Block* foundBlock = nullptr;
    
    // Create and start mining threads
    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < numThreads; i++) {
        threads.push_back(std::thread([&]() {
            Block threadBlock(newIndex, timestamp, data, 0, lastBlock.hash);
            
            while (!blockFound) {
                // Get a new nonce to try
                threadBlock.nonce = sharedNonce.fetch_add(1);
                
                // Calculate hash
                threadBlock.hash = threadBlock.calculateHash();
                
                // Check if hash meets difficulty target
                std::string target(blockchain.difficulty, '0');
                if (threadBlock.hash.substr(0, blockchain.difficulty) == target) {
                    blockFound = true;
                    foundBlock = new Block(threadBlock);
                    break;
                }
            }
        }));
    }
    
    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }
    
    if (foundBlock) {
        // Add the block to the blockchain
        blockchain.addBlock(*foundBlock);
        
        std::cout << "Block mined by: " << minerAddress << std::endl;
        std::cout << "Block hash: " << foundBlock->hash << std::endl;
        std::cout << "Block reward: " << getBlockReward(newIndex) << " PBC" << std::endl;
        
        delete foundBlock;
    }
}

Mizutsi Miner::getBlockReward(uint32_t blockHeight) {
    // Calculate the number of halvings
    uint32_t halvings = blockHeight / HALVING_INTERVAL;
    
    // If we've had too many halvings, reward is 0
    if (halvings >= 64) {
        return 0;
    }
    
    // Initial reward in Mizutsi (50 PBC = 5,000,000,000 Mizutsi)
    Mizutsi initialReward = INITIAL_BLOCK_REWARD * 100000000;
    
    // Apply halvings
    return initialReward >> halvings;
}

} // namespace Pebicoin
