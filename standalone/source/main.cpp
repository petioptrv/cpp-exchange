#include <cxxopts.hpp>
#include <iostream>
#include <string>

#include "config.h"
#include "cppexchange/version.h"

auto main(int argc, char** argv) -> int {
    auto& conf = config::Config::get_instance();

    cxxopts::Options options(*argv, "A program to welcome the world!");

    std::string name;

    // clang-format off
    options.add_options()
    ("h,help", "Show help")
    ("v,version", "Print the current version number")
    ("n,name", "Name to greet", cxxopts::value(name)->default_value("World"));
    // clang-format on

    auto result = options.parse(argc, argv);

    if (result["help"].as<bool>()) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (result["version"].as<bool>()) {
        std::cout << "CPPExchange, version " << CPPEXCHANGE_VERSION << std::endl;
        return 0;
    }

    std::cout << conf.getPostgresDb() << std::endl;

    return 0;
}
