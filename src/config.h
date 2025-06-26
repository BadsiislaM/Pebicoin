#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
#include <string>

namespace Pebicoin {

// Basic coin information
const std::string COIN_NAME = "Pebicoin";
const std::string COIN_TICKER = "PBC";
const std::string GENESIS_BLOCK_DATE = "June 21, 2025";

// Blockchain parameters
const uint32_t BLOCK_TIME = 600;           // 10 minutes in seconds
const uint32_t HALVING_INTERVAL = 170000;  // Blocks until reward halving
const uint32_t STARTING_DIFFICULTY = 0x1d00ffff; // Starting difficulty
const uint32_t DIFFICULTY_ADJUSTMENT_INTERVAL = 2016; // Every ~2 weeks

// Reward parameters
const uint32_t INITIAL_BLOCK_REWARD = 50;  // 50 PBC initial reward
const uint64_t MAX_SUPPLY = 17000000;      // 17 million max supply

// Currency denomination
typedef uint64_t Mizutsi;                  // Base unit (like satoshi)

// 1 PBC = 100,000,000 Mizutsi
const uint64_t MIZUTSI_PER_PBC = 100000000;

} // namespace Pebicoin

#endif // CONFIG_H
