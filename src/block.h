#ifndef BLOCK_H
#define BLOCK_H

#include <cstdint>
#include <string>
#include <ctime>

namespace Pebicoin {

class Block {
public:
    uint32_t index;
    time_t timestamp;
    std::string data;
    uint32_t nonce;
    std::string previousHash;
    std::string hash;

    Block(uint32_t index, time_t timestamp, std::string data, uint32_t nonce, std::string previousHash);
    std::string calculateHash() const;
    void mineBlock(uint32_t difficulty);
};

} // namespace Pebicoin

#endif // BLOCK_H
