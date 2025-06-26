#include "utils.h"
#include "config.h"
#include <sstream>
#include <iomanip>

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
        // Format as PBC
        return formatAmount(amount);
    } else if (amount >= 100000) {
        // Format as MilliPebi (mPBC)
        double mPBC = static_cast<double>(amount) / 100000.0;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(5) << mPBC << " mPBC";
        return ss.str();
    } else if (amount >= 100) {
        // Format as MicroPebi (μPBC)
        double uPBC = static_cast<double>(amount) / 100.0;
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3) << uPBC << " μPBC";
        return ss.str();
    } else {
        // Format as Mizutsi
        std::stringstream ss;
        ss << amount << " Mizutsi";
        return ss.str();
    }
}

} // namespace Pebicoin
