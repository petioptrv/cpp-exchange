//
// Created by Petio Petrov on 2024-07-22.
//

#include "cppexchange/order_data_structures.h"

#include <doctest/doctest.h>

#include "cppexchange/orderbook.h"
#include "cppexchange/order.h"
#include "cppexchange/typedefs.h"

using namespace Orders;

TEST_CASE("Insert and remove orders") {
    OrderBook order_book;

    auto top_ask = order_book.top(SELL, 1);
    auto top_bid = order_book.top(BUY, 1);

    CHECK(top_ask.size() == 0);
    CHECK(top_bid.size() == 0);

    ClientIdT client_id = 0;

    CHECK(order_book.add(client_id, 1, BUY, 9, 2));
    CHECK(order_book.add(client_id, 0, SELL, 10, 1));

    top_ask = order_book.top(SELL, 1);
    top_bid = order_book.top(BUY, 1);

    CHECK(top_ask.size() == 1);
    CHECK(top_ask[0]->quantity == 9);

    CHECK(top_bid.size() == 1);
    CHECK(top_bid[0]->quantity == 10);

    order_book.remove(top_ask[0]->order_id);
    order_book.remove(top_bid[0]->order_id);

    top_ask = order_book.top(SELL, 1);
    top_bid = order_book.top(BUY, 1);

    CHECK(top_ask.size() == 0);
    CHECK(top_bid.size() == 0);

//    CHECK_THROWS_AS
}
