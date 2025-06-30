#include <iostream>
#include <string>
#include <ctime>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>
#include <nlohmann/json.hpp>

#include "blockchain.h"
#include "miner.h"
#include "config.h"
#include "utils.h"
#include "seeds.h"

#define P2P_PORT 24444

using namespace Pebicoin;
using json = nlohmann::json;

std::string getHostname() {
    char hostname[1024];
    gethostname(hostname, sizeof(hostname));
    return std::string(hostname);
}

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

        sendBlockToPeers(blockchain.getLastBlock(), SEED_NODES, P2P_PORT);
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
        inet_pton(AF_INET, "0.0.0.0", &address.sin_addr);
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

        std::string currentHost = getHostname();

        for (const auto& seed : SEED_NODES) {
            if (seed == currentHost) continue;

            struct sockaddr_in seedAddr;
            seedAddr.sin_family = AF_INET;
            seedAddr.sin_port = htons(P2P_PORT);

            struct hostent* host = gethostbyname(seed.c_str());
            if (host == nullptr) continue;

            memcpy(&seedAddr.sin_addr, host->h_addr, host->h_length);

            int sock = socket(AF_INET, SOCK_STREAM, 0);
            if (sock < 0) continue;

            connect(sock, (struct sockaddr*)&seedAddr, sizeof(seedAddr));
            close(sock);
        }

        while (true) {
            new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
            if (new_socket >= 0) {
                char buffer[4096] = {0};
                ssize_t valread = read(new_socket, buffer, sizeof(buffer));
                if (valread > 0) {
                    try {
                        auto j = json::parse(buffer);
                        if (j.contains("type") && j["type"] == "block") {
                            Block receivedBlock(
                                j["index"],
                                j["timestamp"],
                                j["previousHash"],
                                j["hash"],
                                j["nonce"],
                                j["data"]
                            );

                            const Block& latest = blockchain.getLastBlock();
                            if (receivedBlock.previousHash == latest.hash &&
                                receivedBlock.index == latest.index + 1) {
                                blockchain.chain.push_back(receivedBlock);
                                std::cout << "🧱 New block received and added at height: " << receivedBlock.index << std::endl;
                            } else {
                                std::cerr << "⚠️ Invalid block received (wrong previous hash or index)" << std::endl;
                            }
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "❌ Error parsing message: " << e.what() << std::endl;
                    }
                }
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
