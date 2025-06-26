#ifndef HASHER_H
#define HASHER_H

#include <string>

namespace Pebicoin {

std::string doubleSHA256(const std::string& input);

} // namespace Pebicoin

#endif // HASHER_H
