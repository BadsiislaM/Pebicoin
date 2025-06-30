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

    // ✅ Constructor الأصلي المستخدم في التعدين
    Block(uint32_t index, time_t timestamp, std::string data, uint32_t nonce, std::string previousHash);

    // ✅ Constructor جديد لتكوين البلوك من شبكة (تبادل البلوكات)
    Block(uint32_t idx, time_t ts, const std::string& prevHash,
          const std::string& h, uint32_t n, const std::string& d)
        : index(idx), timestamp(ts), previousHash(prevHash), hash(h), nonce(n), data(d) {}

    std::string calculateHash() const;
    void mineBlock(uint32_t difficulty);
};

} // namespace Pebicoin

#endif // BLOCK_H
