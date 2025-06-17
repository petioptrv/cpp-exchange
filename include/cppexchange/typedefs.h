//
// Created by Petio Petrov on 2024-07-22.
//

#pragma once

#include <cstdint>
#include <limits>

#include "utils/time_utils.h"

typedef uint32_t TickerIdT;
typedef uint32_t OrderIdT;
typedef uint32_t TradeIdT;
typedef uint32_t ClientIdT;
typedef uint32_t QuantityT;
typedef uint64_t PriceT;

constexpr auto TickerId_INVALID = std::numeric_limits<OrderIdT>::max();
constexpr auto OrderId_INVALID = std::numeric_limits<OrderIdT>::max();
constexpr auto TradeId_INVALID = std::numeric_limits<TradeIdT>::max();
constexpr auto ClientId_INVALID = std::numeric_limits<ClientIdT>::max();
constexpr auto NsTimestamp_INVALID = std::numeric_limits<Utils::NsTimestampT>::max();
constexpr auto Quantity_INVALID = std::numeric_limits<QuantityT>::max();
constexpr auto Price_INVALID = std::numeric_limits<PriceT>::max();
