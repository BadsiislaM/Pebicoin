#include <iostream>
#include <string>
#include <ctime>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#include "blockchain.h"
#include "miner.h"
#include "config.h"
#include "utils.h"

#define P2P_PORT 24444  // يمكنك تغييره لأي منفذ تريده

using namespace Pebicoin;

// Function to display help message
void displayHelp() {
    std::cout << "Pebicoin Core v1.0 - A standalone cryptocurrency" << std::endl;
    std::cout << "Usage: pebicoin [command] [options]" << std::endl;
    std::cout << "\nCommands:" << std::endl;
    std::cout << "  help                   Display this help message" << std::endl;
    std::cout << "  info                   Display blockchain information" << std::endl;
    std::cout << "  mine <address>         Mine a block with the specified wallet address" << std::endl;
    std::cout << "  createwallet           Create a new wallet" << std::endl;
    std::cout << "  balance <address>      Check the balance of an address" << std::endl;
    std::cout << "  seed                   Run as a seed node on port " << P2P_PORT << std::endl;
}

// Function to display blockchain information
void displayBlockchainInfo(const Blockchain& blockchain) {
    std::cout << "\nPebicoin Blockchain Information:" << std::endl;
    std::cout << "  Name: " << COIN_NAME << " (" << COIN_TICKER << ")" << std::endl;
    std::cout << "  Current height: " << blockchain.chain.size() - 1 << std::endl;
    std::cout << "  Current difficulty: " << blockchain.difficulty << std::endl;
    std::cout << "  Genesis block date: " << GENESIS_BLOCK_DATE << std::endl;
    std::cout << "  Block time: " << BLOCK_TIME << " seconds" << std::endl;
    std::cout << "  Initial block reward: " << INITIAL_BLOCK_REWARD << " PBC" << std::endl;
    std::cout << "  Halving interval: Every " << HALVING_INTERVAL << " blocks" << std::endl;
    std::cout << "  Max supply: " << MAX_SUPPLY << " PBC" << std::endl;

    Block latestBlock = blockchain.getLastBlock();
    std::cout << "\nLatest block:" << std::endl;
    std::cout << "  Height: " << latestBlock.index << std::endl;
    std::cout << "  Hash: " << latestBlock.hash << std::endl;
    std::cout << "  Previous hash: " << latestBlock.previousHash << std::endl;
    std::cout << "  Timestamp: " << std::ctime(&latestBlock.timestamp);
    std::cout << "  Nonce: " << latestBlock.nonce << std::endl;
    std::cout << "  Data: " << latestBlock.data << std::endl;
}

int main(int argc, char* argv[]) {
    Blockchain blockchain;

    if (argc < 2) {
        displayHelp();
        return 0;
    }

    std::string command = argv[1];

    if (command == "help") {
        displayHelp();
    }
    else if (command == "info") {
        displayBlockchainInfo(blockchain);
    }
    else if (command == "mine" && argc >= 3) {
        std::string minerAddress = argv[2];
        std::cout << "Mining a block for address: " << minerAddress << std::endl;

        Miner miner;
        miner.mineBlock(blockchain, minerAddress);

        std::cout << "Block successfully mined!" << std::endl;
        displayBlockchainInfo(blockchain);
    }
    else if (command == "createwallet") {
        std::cout << "This feature requires the wallet module." << std::endl;
        std::cout << "Please use the standalone wallet application." << std::endl;
    }
    else if (command == "balance" && argc >= 3) {
        std::string address = argv[2];
        std::cout << "This feature requires the wallet module." << std::endl;
        std::cout << "Please use the standalone wallet application to check balance for: " << address << std::endl;
    }
    else if (command == "seed") {
        int server_fd, new_socket;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);

        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("socket failed");
            return 1;
        }

        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(P2P_PORT);

        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            perror("bind failed");
            return 1;
        }

        if (listen(server_fd, 3) < 0) {
            perror("listen");
            return 1;
        }

        std::cout << "✅ Pebicoin Seed Node is running on port " << P2P_PORT << "..." << std::endl;

        while (true) {
            new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
            if (new_socket >= 0) {
                std::cout << "🔌 New peer connected" << std::endl;
                close(new_socket);
            }
        }
    }
    else {
        std::cout << "Unknown command or missing arguments." << std::endl;
        displayHelp();
        return 1;
    }
    return 0;
}
