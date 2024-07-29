//
// Created by Petio Petrov on 2024-06-15.
//

#pragma once

#include <filesystem>

namespace Constants {
    static const std::filesystem::path PROJECT_ROOT = std::filesystem::current_path() / "..";
    constexpr size_t ME_MAX_ORDER_IDS = 1024 * 1024;
}  // namespace constants
