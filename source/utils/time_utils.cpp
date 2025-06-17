//
// Created by Petio Petrov on 2024-10-13.
//

#include "utils/time_utils.h"

#include <chrono>

Common::NsTimestampT Common::getCurrentNsTimestamp() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::system_clock::now().time_since_epoch()
    )
        .count();
}
