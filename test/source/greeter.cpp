#include <cppexchange/version.h>
#include <doctest/doctest.h>

#include <string>

#include "cppexchange/cppexchange.h"

TEST_CASE("CPPExchange") {
    using namespace cppexchange;

    CPPExchange cppexchange("Tests");

    CHECK(cppexchange.greet(LanguageCode::EN) == "Hello, Tests!");
    CHECK(cppexchange.greet(LanguageCode::DE) == "Hallo Tests!");
    CHECK(cppexchange.greet(LanguageCode::ES) == "¡Hola Tests!");
    CHECK(cppexchange.greet(LanguageCode::FR) == "Bonjour Tests!");
}

TEST_CASE("CPPExchange version") {
    static_assert(std::string_view(CPPEXCHANGE_VERSION) == std::string_view("1.0"));
    CHECK(std::string(CPPEXCHANGE_VERSION) == std::string("1.0"));
}
