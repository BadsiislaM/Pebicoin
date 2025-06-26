#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "config.h"

namespace Pebicoin {

Mizutsi toMizutsi(double pbc);
double fromMizutsi(Mizutsi m);
std::string formatAmount(Mizutsi amount);
std::string formatAmountWithUnit(Mizutsi amount);

} // namespace Pebicoin

#endif // UTILS_H
