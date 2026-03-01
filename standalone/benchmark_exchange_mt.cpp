#include "cppexchange/cppexchange.h"
#include "cppexchange/order.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

using namespace CPPExchange;
using namespace Orders;
using namespace Constants;

using Clock = std::chrono::steady_clock;
using ns = std::chrono::nanoseconds;

constexpr size_t TOTAL_ORDERS = 5'000'000;
constexpr size_t MAX_IN_FLIGHT = 1;
constexpr size_t WARMUP_ORDERS = 5'000;
constexpr size_t NUM_PRODUCERS = 1;

static long long now_ns() {
    return std::chrono::duration_cast<ns>(Clock::now().time_since_epoch()).count();
}

int main() {
#ifdef __aarch64__
    const char* arch = "ARM64";
#elif defined(__x86_64__)
    const char* arch = "x86_64";
#else
    const char* arch = "Unknown";
#endif

    std::cout << "Architecture: " << arch << "\n";
    std::cout << "Producers: " << NUM_PRODUCERS << "\n";
    std::cout << "Warmup: " << WARMUP_ORDERS << "\n";
    std::cout << "Total Orders: " << TOTAL_ORDERS << "\n";
    std::cout << "Max In-Flight: " << MAX_IN_FLIGHT << "\n\n";

    constexpr size_t Q = 1ull << 21;
    ClientRequestLFQueue  req_q(Q);
    ClientResponseLFQueue resp_q(Q);
    MarketUpdateLFQueue   mkt_q(Q);

    TickerIdT ticker_id = 0;

    Exchange exchange(&req_q, &resp_q, &mkt_q, {ticker_id});
    exchange.start();

    std::cout << "Warming up...\n";
    for (size_t i = 0; i < WARMUP_ORDERS; ++i) {
        req_q.push({RequestType::ADD, ticker_id, 0, OrderId_INVALID, OrderSide::BUY, 1, 100});
        while (!resp_q.pop()) {}
    }

    std::vector<long long> send_ts(TOTAL_ORDERS);
    std::vector<long long> latencies;
    latencies.reserve(TOTAL_ORDERS);

    std::atomic<size_t> sent{0};
    std::atomic<size_t> received{0};
    std::atomic<bool> go{false};

    const size_t order_id_base = WARMUP_ORDERS;

    std::thread consumer([&]() {
        while (!go.load(std::memory_order_acquire)) {}

        while (received.load(std::memory_order_relaxed) < TOTAL_ORDERS) {
            ClientResponse* resp = resp_q.pop();
            const long long t1 = now_ns();

            if (!resp) {
                continue;
            }
            received.fetch_add(1, std::memory_order_relaxed);
            if (resp->type != ResponseType::ACCEPTED) {
                continue;
            }

            const auto oid0 = static_cast<size_t>(resp->order_id - 1);
            if (oid0 < order_id_base) continue;

            const size_t idx = oid0 - order_id_base;
            if (idx >= TOTAL_ORDERS) continue;

            latencies.push_back(t1 - send_ts[idx]);
        }
    });

    std::vector<std::thread> producers;
    producers.reserve(NUM_PRODUCERS);

    for (size_t p = 0; p < NUM_PRODUCERS; ++p) {
        producers.emplace_back([&, p]() {
            while (!go.load(std::memory_order_acquire)) {}

            while (true) {
                while ((sent.load(std::memory_order_relaxed) - received.load(std::memory_order_relaxed)) >= MAX_IN_FLIGHT) {}

                const size_t idx = sent.fetch_add(1, std::memory_order_relaxed);
                if (idx >= TOTAL_ORDERS) {
                    break;
                }

                send_ts[idx] = now_ns();
                req_q.push({
                    RequestType::ADD,
                    ticker_id,
                    static_cast<ClientIdT>(p),
                    OrderId_INVALID,
                    OrderSide::BUY,
                    1,
                    100
                });
            }
        });
    }

    std::cout << "Benchmarking...\n";
    const auto t0 = Clock::now();
    go.store(true, std::memory_order_release);

    for (auto& t : producers) t.join();
    consumer.join();
    const auto t1 = Clock::now();

    exchange.stop();

    const double seconds = std::chrono::duration<double>(t1 - t0).count();
    const double throughput = static_cast<double>(TOTAL_ORDERS) / seconds;

    std::sort(latencies.begin(), latencies.end());
    auto pct = [&](double p) -> long long {
        const auto i = static_cast<size_t>(p * (latencies.size() - 1));
        return latencies[i];
    };

    std::cout << "\nRESULTS\n";
    std::cout << "Elapsed: " << seconds << " s\n";
    std::cout << "Throughput: " << throughput << " orders/sec\n";
    std::cout << "p50:  " << pct(0.50)  << " ns\n";
    std::cout << "p99:  " << pct(0.99)  << " ns\n";
    std::cout << "p999: " << pct(0.999) << " ns\n";

    return 0;
}