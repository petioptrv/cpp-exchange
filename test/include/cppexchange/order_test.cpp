//
// Created by Petio Petrov on 2024-07-17.
//

#include "cppexchange/order.h"
#include "cppexchange/helpers.h"

#include <doctest/doctest.h>

using namespace Orders;
using namespace Helpers;

TEST_CASE("Order properties") {
    TickerIdT ticker_id = 0;
    Order order(ticker_id, 0, 1, getCurrentNsTimestamp(), OrderSide::BUY, 4, 5);

    CHECK(order.order_id == 0);
    CHECK(order.client_id == 1);
    CHECK(order.side == OrderSide::BUY);
    CHECK(order.quantity == 4);
    CHECK(order.executed == 0);
}

TEST_CASE("OrdersAtPriceLevel adding and removing orders") {
    OrdersPriceLevel price_level;

    int order_count = 0;
    TickerIdT ticker_id = 0;
    OrderIdT first_order_id = 0;
    NsTimestampT initial_ns_timestamp = getCurrentNsTimestamp();
    ClientIdT client_id = 2;
    QuantityT top_of_book_quantity = 5;
    PriceT price = 10;

    price_level.setPriceLevel(price);

    CHECK(price_level.top() == nullptr);

    Order order0(
        ticker_id,
        first_order_id + order_count,
        client_id,
        initial_ns_timestamp + std::chrono::milliseconds(order_count * 1000).count(),
        OrderSide::BUY,
        top_of_book_quantity + order_count,
        price
    );
    price_level.add(&order0);

    CHECK(price_level.top() == &order0);

    ++order_count;
    Order order1(
        ticker_id,
        first_order_id + order_count,
        client_id,
        initial_ns_timestamp + std::chrono::milliseconds(order_count * 1000).count(),
        OrderSide::BUY,
        top_of_book_quantity + order_count,
        price
    );
    price_level.add(&order1);

    CHECK(price_level.top() == &order0);

    price_level.cancel(order0.order_id);

    CHECK(price_level.top() == &order1);

    price_level.cancel(order1.order_id);

    CHECK(price_level.top() == nullptr);
}

TEST_CASE("OrdersAtPriceLevel chaining levels") {
    OrdersPriceLevel price_level0;
    OrdersPriceLevel price_level1;

    price_level0.setNextPriceLevel(&price_level1);

    CHECK(price_level0.getNextPriceLevel() == &price_level1);
}
