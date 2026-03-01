//
// Created by Petio Petrov on 2024-06-15.
//

#pragma once

#include <filesystem>

namespace Constants {
    static const std::filesystem::path PROJECT_ROOT = std::filesystem::current_path() / "..";

    constexpr size_t MAX_NUM_CLIENTS = 256;
    constexpr size_t MAX_TICKERS = 16;
    constexpr size_t MAX_ORDER_IDS = 1 << 23;
    constexpr size_t MAX_PRICE_LEVELS = 128 * 1024;

    constexpr size_t UPDATES_QUEUE_SIZE = 1 << 23;
    constexpr size_t REQUESTS_QUEUE_SIZE = 1 << 23;
}  // namespace Constants
