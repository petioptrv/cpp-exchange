//
// Created by Petio Petrov on 2024-06-15.
//

#pragma once

#include <filesystem>

namespace Constants {
    static const std::filesystem::path PROJECT_ROOT = std::filesystem::current_path() / "..";

    constexpr size_t MAX_NUM_CLIENTS = 256;
    constexpr size_t MAX_ORDER_IDS = 1024 * 1024;
    constexpr size_t MAX_PRICE_LEVELS
        = 256;  // todo: check this... it can be hackable since we use it to index the
                // orders-by-price by modding the price by it, but this means we can set a price so
                // extreme, it loops back around and goes to the front of the queue with an extreme
                // price
}  // namespace Constants
