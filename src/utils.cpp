#include "utils.h"
#include "config.h"
#include "blockchain.h"
#include <sstream>
#include <iomanip>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <nlohmann/json.hpp> // مكتبة JSON

using json = nlohmann::json;

namespace Pebicoin {

Mizutsi toMizutsi(double pbc) {
    return static_cast<Mizutsi>(pbc * 100000000);
}

double fromMizutsi(Mizutsi m) {
    return static_cast<double>(m) / 100000000.0;
}

std::string formatAmount(Mizutsi amount) {
    double pbc = fromMizutsi(amount);
    std::stringstream ss;
    ss << std::fixed << std::setprecision(8) << pbc << " PBC";
    return ss.str();
}

std::string formatAmountWithUnit(Mizutsi amount) {
    if (amount >= 100000000) {
        return formatAmount(amount);
    } else if (amount >= 100000) {
        double mPBC = static_cast<double>(amount) / 100000.0;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(5) << mPBC << " mPBC";
        return ss.str();
    } else if (amount >= 100) {
        double uPBC = static_cast<double>(amount) / 100.0;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3) << uPBC << " μPBC";
        return ss.str();
    } else {
        std::stringstream ss;
        ss << amount << " Mizutsi";
        return ss.str();
    }
}

// 🔗 إرسال بلوك إلى كل العقد
void sendBlockToPeers(const Block& block, const std::vector<std::string>& peers, int port) {
    json jblock = {
        {"type", "block"},
        {"index", block.index},
        {"timestamp", block.timestamp},
        {"previousHash", block.previousHash},
        {"hash", block.hash},
        {"nonce", block.nonce},
        {"data", block.data}
    };

    std::string message = jblock.dump();

    for (const auto& peer : peers) {
        struct sockaddr_in peerAddr;
        peerAddr.sin_family = AF_INET;
        peerAddr.sin_port = htons(port);

        struct hostent* host = gethostbyname(peer.c_str());
        if (!host) {
            std::cerr << "❌ Failed to resolve: " << peer << std::endl;
            continue;
        }

        memcpy(&peerAddr.sin_addr, host->h_addr, host->h_length);

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            std::cerr << "❌ Socket error to: " << peer << std::endl;
            continue;
        }

        if (connect(sock, (struct sockaddr*)&peerAddr, sizeof(peerAddr)) == 0) {
            send(sock, message.c_str(), message.size(), 0);
            std::cout << "📤 Block sent to peer: " << peer << std::endl;
        } else {
            std::cerr << "⚠️ Could not connect to: " << peer << std::endl;
        }

        close(sock);
    }
}

} // namespace Pebicoin
