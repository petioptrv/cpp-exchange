#include <cpp_exchange/version.h>

#include <cxxopts.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

#include "cpp_exchange/cpp_exchange.h"

auto main(int argc, char** argv) -> int {
  const std::unordered_map<std::string, cpp_exchange::LanguageCode> languages{
      {"en", cpp_exchange::LanguageCode::EN},
      {"de", cpp_exchange::LanguageCode::DE},
      {"es", cpp_exchange::LanguageCode::ES},
      {"fr", cpp_exchange::LanguageCode::FR},
  };

  cxxopts::Options options(*argv, "A program to welcome the world!");

  std::string language;
  std::string name;

  // clang-format off
  options.add_options()
    ("h,help", "Show help")
    ("v,version", "Print the current version number")
    ("n,name", "Name to greet", cxxopts::value(name)->default_value("World"))
    ("l,lang", "Language code to use", cxxopts::value(language)->default_value("en"))
  ;
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

  auto langIt = languages.find(language);
  if (langIt == languages.end()) {
    std::cerr << "unknown language code: " << language << std::endl;
    return 1;
  }

  cpp_exchange::CPPExchange cpp_exchange(name);
  std::cout << cpp_exchange.greet(langIt->second) << std::endl;

  return 0;
}
