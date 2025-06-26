#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <vector>
#include "block.h"

namespace Pebicoin {

class Blockchain {
public:
    std::vector<Block> chain;
    uint32_t difficulty;
    time_t lastDifficultyAdjustmentTime;

    Blockchain();
    void addBlock(Block newBlock);
    bool isChainValid() const;
    Block getLastBlock() const;
    void adjustDifficulty();
};

} // namespace Pebicoin

#endif // BLOCKCHAIN_H
