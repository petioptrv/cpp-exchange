//
// Created by Petio Petrov on 2024-07-31.
//

#include "cppexchange/helpers.h"

NsTimestampT Helpers::getCurrentNsTimestamp() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
                   std::chrono::system_clock::now().time_since_epoch()
        )
            .count();
}

std::string
Helpers::getEnvWithDefault(const std::string& variable_name, const std::string& default_value) {
    const char* value = getenv(variable_name.c_str());
    return value ? value : default_value;
}
