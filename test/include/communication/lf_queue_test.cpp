//
// Created by Petio Petrov on 2024-07-17.
//
#include <doctest/doctest.h>

#include "communication/lf_queue.h"

TEST_CASE("LFQueue consume empty returns nullptr") {
    using namespace Communication;

    LFQueue<int*> queue(10);

    CHECK(queue.pop() == nullptr);
}

TEST_CASE("LFQueue consume and produce no overflow") {
    using namespace Communication;

    struct Int {
        int i;
    };
    int n = 10;
    LFQueue<Int> queue(n);

    for (int i = 0; i != n; ++i) {
        queue.push(Int{i});
    }

    for (int i = 0; i != n; ++i) {
        CHECK(queue.pop()->i == i);
    }
}
