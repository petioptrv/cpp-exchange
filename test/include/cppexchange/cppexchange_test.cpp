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

TEST_CASE("Add limit order") {
    string spy = "SPY";
    string qqq = "QQQ";
    Exchange exchange{{spy, qqq}};

    ClientIdT client_id = 2;
    QuantityT top_of_book_quantity = 5;
    PriceT price = 10;

    auto order_spy = exchange.addOrder(spy, client_id, BUY, top_of_book_quantity, price - 1);

    CHECK(order_spy->ms_timestamp != MsTimestamp_INVALID);
    CHECK(order_spy->order_id == 1);

    auto top_bid_spy = exchange.top(spy, BUY);
    auto top_bid_qqq = exchange.top(qqq, BUY);

    CHECK(top_bid_spy == order_spy);
    CHECK(top_bid_qqq == nullptr);

    auto order_qqq = exchange.addOrder(qqq, client_id, SELL, top_of_book_quantity, price + 2);

    CHECK(order_qqq->ms_timestamp != MsTimestamp_INVALID);
    CHECK(order_qqq->ms_timestamp >= order_qqq->ms_timestamp);
    CHECK(order_qqq->order_id == 2);

    auto top_ask_qqq = exchange.top(qqq, SELL);
    auto top_ask_spy = exchange.top(spy, SELL);

    CHECK(top_ask_qqq == order_qqq);
    CHECK(top_ask_spy == nullptr);
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
