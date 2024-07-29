//
// Created by Petio Petrov on 2024-07-20.
//

#include "cppexchange/order_data_structures.h"

#include <doctest/doctest.h>

#include "cppexchange/order.h"

using namespace Orders;
using namespace OrderDataStructures;

TEST_CASE("Sell orders properly sorted by heap") {
    SellOrderQueue s;

    s.push(new Order(0, 0, 2, SELL, 1, 9));
    s.push(new Order(1, 0, 3, SELL, 1, 8));
    s.push(new Order(2, 0, 1, SELL, 3, 9));
    s.push(new Order(3, 0, 4, SELL, 2, 10));

    CHECK(s.top()->order_id == 1);
    s.pop();
    CHECK(s.top()->order_id == 2);
    s.pop();
    CHECK(s.top()->order_id == 0);
    s.pop();
    CHECK(s.top()->order_id == 3);
}

TEST_CASE("Buy orders properly sorted by heap") {
    BuyOrderQueue s;

    s.push(new Order(0, 0, 2, BUY, 1, 9));
    s.push(new Order(1, 0, 3, BUY, 1, 8));
    s.push(new Order(2, 0, 1, BUY, 3, 9));
    s.push(new Order(3, 0, 4, BUY, 2, 10));

    CHECK(s.top()->order_id == 3);
    s.pop();
    CHECK(s.top()->order_id == 2);
    s.pop();
    CHECK(s.top()->order_id == 0);
    s.pop();
    CHECK(s.top()->order_id == 1);
}

TEST_CASE("Orders iteration") {
    BuyOrderQueue s;
    OrdersIterator oit;

    s.push(new Order(0, 0, 2, BUY, 1, 9));
    s.push(new Order(1, 0, 3, BUY, 1, 8));
    s.push(new Order(2, 0, 1, BUY, 3, 9));
    s.push(new Order(3, 0, 4, BUY, 2, 10));

    oit = s.begin();
    CHECK((*oit)->order_id == 3);

    ++oit;
    CHECK((*oit)->order_id == 2);

    ++oit;
    ++oit;
    ++oit;
    CHECK(oit == s.end());
}
