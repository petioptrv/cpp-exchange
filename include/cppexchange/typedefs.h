//
// Created by Petio Petrov on 2024-07-22.
//

#pragma once

#include <cstdint>
#include <limits>

typedef uint32_t OrderIdT;
typedef uint32_t ClientIdT;
typedef uint64_t MsTimestampT;
typedef float QuantityT;
typedef float PriceT;

constexpr auto OrderId_INVALID = std::numeric_limits<OrderIdT>::max();
constexpr auto ClientId_INVALID = std::numeric_limits<ClientIdT>::max();
constexpr auto MsTimestamp_INVALID = std::numeric_limits<MsTimestampT>::max();
constexpr auto Quantity_INVALID = std::numeric_limits<QuantityT>::max();
constexpr auto Price_INVALID = std::numeric_limits<PriceT>::max();