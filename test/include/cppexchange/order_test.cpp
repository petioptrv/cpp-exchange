//
// Created by Petio Petrov on 2024-07-17.
//

#include "cppexchange/order.h"
#include "cppexchange/helpers.h"

#include <doctest/doctest.h>

using namespace Orders;
using namespace Helpers;

TEST_CASE("Order properties") {
    Order order(0, 1, getCurrentMsTimestamp(), BUY, 4, 5);

    CHECK(order.order_id == 0);
    CHECK(order.client_id == 1);
    CHECK(order.side == BUY);
    CHECK(order.quantity == 4);
    CHECK(order.executed == 0);
}

TEST_CASE("OrdersAtPriceLevel adding and removing orders") {
    OrdersPriceLevel price_level;

    int order_count = 0;
    OrderIdT first_order_id = 0;
    MsTimestampT initial_ms_timestamp = getCurrentMsTimestamp();
    ClientIdT client_id = 2;
    QuantityT top_of_book_quantity = 5;
    PriceT price = 10;

    price_level.setPriceLevel(price);

    CHECK(price_level.top() == nullptr);

    Order order0(
        first_order_id + order_count,
        client_id,
        initial_ms_timestamp + std::chrono::milliseconds(order_count * 1000),
        BUY,
        top_of_book_quantity + order_count,
        price
    );
    price_level.add(&order0);

    CHECK(price_level.top() == &order0);

    ++order_count;
    Order order1(
        first_order_id + order_count,
        client_id,
        initial_ms_timestamp + std::chrono::milliseconds(order_count * 1000),
        BUY,
        top_of_book_quantity + order_count,
        price
    );
    price_level.add(&order1);

    CHECK(price_level.top() == &order0);

    price_level.remove(&order0);

    CHECK(price_level.top() == &order1);

    price_level.remove(&order1);

    CHECK(price_level.top() == nullptr);
}

TEST_CASE("OrdersAtPriceLevel chaining levels") {
    OrdersPriceLevel price_level0;
    OrdersPriceLevel price_level1;

    price_level0.setNextPriceLevel(&price_level1);

    CHECK(price_level0.getNextPriceLevel() == &price_level1);
}
