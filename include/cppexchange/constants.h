//
// Created by Petio Petrov on 2024-06-15.
//

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <filesystem>

namespace constants {
    static const std::filesystem::path PROJECT_ROOT = std::filesystem::current_path() / "..";
}  // namespace constants

#endif  // CONSTANTS_H
