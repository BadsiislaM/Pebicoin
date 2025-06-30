#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include "config.h"
#include "block.h"

namespace Pebicoin {

Mizutsi toMizutsi(double pbc);
double fromMizutsi(Mizutsi m);
std::string formatAmount(Mizutsi amount);
std::string formatAmountWithUnit(Mizutsi amount);

// ✅ إرسال الكتلة إلى باقي العقد عبر الشبكة
void sendBlockToPeers(const Block& block, const std::vector<std::string>& peers, int port);

} // namespace Pebicoin

#endif // UTILS_H
