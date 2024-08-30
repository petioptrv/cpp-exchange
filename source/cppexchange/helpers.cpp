//
// Created by Petio Petrov on 2024-07-31.
//

#include "cppexchange/helpers.h"

MsTimestampT Helpers::getCurrentMsTimestamp() {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );
    return ms;
}

std::string
Helpers::getEnvWithDefault(const std::string& variable_name, const std::string& default_value) {
    const char* value = getenv(variable_name.c_str());
    return value ? value : default_value;
}
