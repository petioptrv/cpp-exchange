//
// Created by Petio Petrov on 2024-07-17.
//
#include <doctest/doctest.h>
#include <thread>

#include "communication/lf_queue.h"

struct Int {
    int i;
};

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

void threaded_produce(Communication::LFQueue<Int>& q) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    q.push(Int{2});
}

TEST_CASE("LFQueue consume and produce threaded") {
    using namespace Communication;

    LFQueue<Int> q(1);

    std::thread t(threaded_produce, std::ref(q));

    auto res = q.pop();

    CHECK(res == nullptr);

    t.join();

    res = q.pop();

    CHECK(res->i == 2);
}
