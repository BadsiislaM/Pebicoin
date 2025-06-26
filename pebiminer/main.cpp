#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <vector>
#include <atomic>
#include <chrono>
#include <csignal>
#include "../src/blockchain.h"
#include "../src/miner.h"
#include "../src/config.h"
#include "../src/utils.h"

using namespace Pebicoin;

// Global variables for signal handling
std::atomic<bool> running(true);
Blockchain* globalBlockchain = nullptr;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    running = false;
}

// Function to display help message
void displayHelp(const char* programName) {
    std::cout << "Pebicoin Miner - CLI mining tool for Pebicoin cryptocurrency" << std::endl;
    std::cout << "Usage: " << programName << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --address=<addr>    Wallet address to receive mining rewards (required)" << std::endl;
    std::cout << "  --threads=<num>     Number of mining threads (default: 1)" << std::endl;
    std::cout << "  --help              Display this help message" << std::endl;
}

// Function to parse command line arguments
bool parseArgs(int argc, char* argv[], std::string& address, uint32_t& threads) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg.find("--address=") == 0) {
            address = arg.substr(10);
        } else if (arg.find("--threads=") == 0) {
            try {
                threads = std::stoi(arg.substr(10));
            } catch (const std::exception&) {
                std::cerr << "Invalid thread count: " << arg.substr(10) << std::endl;
                return false;
            }
        } else if (arg == "--help") {
            displayHelp(argv[0]);
            return false;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            displayHelp(argv[0]);
            return false;
        }
    }
    
    if (address.empty()) {
        std::cerr << "Error: Wallet address is required" << std::endl;
        displayHelp(argv[0]);
        return false;
    }
    
    return true;
}

// Main mining loop
void miningLoop(Blockchain& blockchain, const std::string& address, uint32_t threads) {
    Miner miner;
    uint32_t blocksMined = 0;
    
    std::cout << "Starting Pebicoin miner with " << threads << " threads" << std::endl;
    std::cout << "Mining to address: " << address << std::endl;
    std::cout << "Press Ctrl+C to stop mining" << std::endl;
    
    auto startTime = std::chrono::steady_clock::now();
    
    while (running) {
        std::cout << "\nMining block #" << blockchain.chain.size() << "..." << std::endl;
        
        // Mine a block with multiple threads
        miner.mineBlockMultiThreaded(blockchain, address, threads);
        
        // Block successfully mined
        blocksMined++;
        
        // Calculate hashrate
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(
            currentTime - startTime).count();
        
        if (elapsedSeconds > 0) {
            double hashrate = static_cast<double>(blocksMined) / elapsedSeconds;
            std::cout << "Mining statistics:" << std::endl;
            std::cout << "  Blocks mined: " << blocksMined << std::endl;
            std::cout << "  Running time: " << elapsedSeconds << " seconds" << std::endl;
            std::cout << "  Average hashrate: " << hashrate << " blocks/s" << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    // Default values
    std::string minerAddress;
    uint32_t threads = 1;
    
    // Parse command line arguments
    if (!parseArgs(argc, argv, minerAddress, threads)) {
        return 1;
    }
    
    // Set up signal handler for graceful shutdown
    std::signal(SIGINT, signalHandler);
    
    // Create blockchain
    Blockchain blockchain;
    globalBlockchain = &blockchain;
    
    // Display miner information
    std::cout << "Pebicoin Miner v1.0" << std::endl;
    std::cout << "Coin: " << COIN_NAME << " (" << COIN_TICKER << ")" << std::endl;
    std::cout << "Block reward: " << INITIAL_BLOCK_REWARD << " PBC" << std::endl;
    std::cout << "Difficulty: " << blockchain.difficulty << std::endl;
    
    // Start mining
    miningLoop(blockchain, minerAddress, threads);
    
    std::cout << "Miner stopped. Goodbye!" << std::endl;
    return 0;
}