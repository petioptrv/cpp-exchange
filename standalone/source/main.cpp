#include <cxxopts.hpp>
#include <iostream>
#include <string>

#include "cppexchange/db/postgresql.h"
#include "cppexchange/version.h"

auto main(int argc, char** argv) -> int {

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

    try {
        PostgreSQL* db = PostgreSQLBuilder().get_connection_instance();
        if (db->is_connected()) {
            std::cout << "Opened database successfully" << std::endl;
        } else {
            std::cout << "Can't open database" << std::endl;
            return 1;
        }
        delete db;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
