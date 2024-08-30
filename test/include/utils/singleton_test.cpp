//
// Created by Petio Petrov on 2024-07-20.
//

#include "utils/singleton.h"

#include <doctest/doctest.h>

TEST_CASE("Singleton") {
    class Mock : public Utils::Singleton<Mock> {
      public:
        int a = 0;
    };

    Mock& m0 = Mock::getInstance();
    m0.a = 2;
    Mock& m1 = Mock::getInstance();

    CHECK(m1.a == 2);
}
