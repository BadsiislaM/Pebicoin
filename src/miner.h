#ifndef MINER_H
#define MINER_H

#include "blockchain.h"
#include "config.h"

namespace Pebicoin {

class Miner {
public:
    void mineBlock(Blockchain& blockchain, std::string minerAddress);
    void mineBlockMultiThreaded(Blockchain& blockchain, std::string minerAddress, uint32_t numThreads);
    
    // Calculate block reward based on block height
    Mizutsi getBlockReward(uint32_t blockHeight);
};

} // namespace Pebicoin

#endif // MINER_H
