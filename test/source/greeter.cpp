#include <doctest/doctest.h>
#include <cpp_exchange/version.h>

#include <string>

#include "cpp_exchange/cpp_exchange.h"

TEST_CASE("CPPExchange") {
  using namespace cpp_exchange;

  CPPExchange cpp_exchange("Tests");

  CHECK(cpp_exchange.greet(LanguageCode::EN) == "Hello, Tests!");
  CHECK(cpp_exchange.greet(LanguageCode::DE) == "Hallo Tests!");
  CHECK(cpp_exchange.greet(LanguageCode::ES) == "¡Hola Tests!");
  CHECK(cpp_exchange.greet(LanguageCode::FR) == "Bonjour Tests!");
}

TEST_CASE("CPPExchange version") {
  static_assert(std::string_view(CPPEXCHANGE_VERSION) == std::string_view("1.0"));
  CHECK(std::string(CPPEXCHANGE_VERSION) == std::string("1.0"));
}
