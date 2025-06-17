//
// Created by Petio Petrov on 2024-10-13.
//

#pragma once
#include <cstdint>

namespace Utils {
    typedef long long NsTimestampT;

    constexpr NsTimestampT NANOS_TO_MICROS = 1000;
    constexpr NsTimestampT MICROS_TO_MILLIS = 1000;
    constexpr NsTimestampT MILLIS_TO_SECS = 1000;
    constexpr NsTimestampT NANOS_TO_MILLIS = NANOS_TO_MICROS * MICROS_TO_MILLIS;
    constexpr NsTimestampT NANOS_TO_SECS = NANOS_TO_MILLIS * MILLIS_TO_SECS;

    NsTimestampT getCurrentNsTimestamp();
}  // namespace Common
