#pragma once

/// Creates a bitmask from a bit number.
#define BIT(n) (1U << (n))

#include <tuple>
using key_type = std::pair<std::size_t, bool>; /// size_t - virtual key code, bool - is the key extended flag
