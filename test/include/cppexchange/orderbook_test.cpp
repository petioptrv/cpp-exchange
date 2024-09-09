//
// Created by Petio Petrov on 2024-07-22.
//

#include <doctest/doctest.h>

#include "cppexchange/orderbook.h"
#include "cppexchange/order.h"
#include "cppexchange/typedefs.h"

using namespace Orders;
using namespace CPPExchange;

TEST_CASE("Empty order book") {
    OrderBook order_book(0);

    auto top_ask = order_book.top(SELL);
    auto top_bid = order_book.top(BUY);

    CHECK(top_ask == nullptr);
    CHECK(top_bid == nullptr);
}

TEST_CASE("Insert one bid and one ask") {
    OrderBook order_book(0);

    ClientIdT client_id = 2;
    QuantityT top_of_book_quantity = 5;
    PriceT price = 10;

    auto order0 = order_book.add(client_id, BUY, top_of_book_quantity, price - 1);

    CHECK(order0->ms_timestamp != MsTimestamp_INVALID);
    CHECK(order0->order_id == 1);

    auto top_bid = order_book.top(BUY);

    CHECK(top_bid == order0);

    auto order1 = order_book.add(client_id, SELL, top_of_book_quantity, price + 2);

    CHECK(order1->ms_timestamp != MsTimestamp_INVALID);
    CHECK(order1->ms_timestamp >= order0->ms_timestamp);
    CHECK(order1->order_id == 2);

    auto top_ask = order_book.top(SELL);

    CHECK(top_ask == order1);
}

TEST_CASE("Insert second level order") {
    OrderBook order_book(0);

    ClientIdT client_id = 2;
    QuantityT top_of_book_quantity = 5;
    PriceT price = 10;

    order_book.add(client_id, BUY, top_of_book_quantity, price - 2);
    auto order1 = order_book.add(client_id, BUY, top_of_book_quantity + 1, price - 1);

    auto top_bid = order_book.top(BUY);

    CHECK(top_bid == order1);
}

TEST_CASE("Remove order") {
    OrderBook order_book(0);

    ClientIdT client_id = 2;
    QuantityT top_of_book_quantity = 5;
    PriceT price = 10;

    auto order0 = order_book.add(client_id, BUY, top_of_book_quantity, price - 2);
    auto order1 = order_book.add(client_id, BUY, top_of_book_quantity + 1, price - 1);
    auto order2 = order_book.add(client_id, BUY, top_of_book_quantity + 1, price - 1);

    auto top_bid = order_book.top(BUY);

    CHECK(top_bid == order1);

    order_book.remove(order1);
    top_bid = order_book.top(BUY);

    CHECK(top_bid == order2);

    order_book.remove(order2);
    top_bid = order_book.top(BUY);

    CHECK(top_bid == order0);
}
