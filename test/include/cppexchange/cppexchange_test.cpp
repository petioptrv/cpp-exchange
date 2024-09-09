//
// Created by Petio Petrov on 2024-08-30.
//

#include "cppexchange/cppexchange.h"

#include <doctest/doctest.h>

#include <string>

#include "cppexchange/order.h"

using namespace std;
using namespace CPPExchange;
using namespace Orders;

TEST_CASE("Add ticker") {
    Exchange exchange;
    string ticker = "SPY";

    exchange.addTicker(ticker);

    CHECK(exchange.top(ticker, SELL) == nullptr);
    CHECK(exchange.top(ticker, BUY) == nullptr);
}

TEST_CASE("Add order") {
    string ticker = "SPY";
    Exchange exchange{{ticker}};

    ClientIdT client_id = 2;
    QuantityT top_of_book_quantity = 5;
    PriceT price = 10;

    auto order0 = exchange.addOrder(ticker, client_id, BUY, top_of_book_quantity, price - 1);

    CHECK(order0->ms_timestamp != MsTimestamp_INVALID);
    CHECK(order0->order_id == 1);

    auto top_bid = exchange.top(ticker, BUY);

    CHECK(top_bid == order0);

    auto order1 = exchange.addOrder(ticker, client_id, SELL, top_of_book_quantity, price + 2);

    CHECK(order1->ms_timestamp != MsTimestamp_INVALID);
    CHECK(order1->ms_timestamp >= order0->ms_timestamp);
    CHECK(order1->order_id == 2);

    auto top_ask = exchange.top(ticker, SELL);

    CHECK(top_ask == order1);
}

TEST_CASE("Remove order") {
    string ticker = "SPY";
    Exchange exchange{{ticker}};

    ClientIdT client_id = 2;
    QuantityT top_of_book_quantity = 5;
    PriceT price = 10;

    auto order0 = exchange.addOrder(ticker, client_id, BUY, top_of_book_quantity, price - 2);
    auto order1 = exchange.addOrder(ticker, client_id, BUY, top_of_book_quantity + 1, price - 1);
    auto order2 = exchange.addOrder(ticker, client_id, BUY, top_of_book_quantity + 1, price - 1);

    auto top_bid = exchange.top(ticker, BUY);

    CHECK(top_bid == order1);

    exchange.removeOrder(order1);
    top_bid = exchange.top(ticker, BUY);

    CHECK(top_bid == order2);

    exchange.removeOrder(order2);
    top_bid = exchange.top(ticker, BUY);

    CHECK(top_bid == order0);
}
