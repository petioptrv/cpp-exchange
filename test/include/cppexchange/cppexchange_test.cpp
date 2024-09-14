//
// Created by Petio Petrov on 2024-08-30.
//

#include "cppexchange/cppexchange.h"

#include <doctest/doctest.h>

#include <string>

#include "cppexchange/order.h"
#include "cppexchange/constants.h"

using namespace std;
using namespace CPPExchange;
using namespace Orders;
using namespace Constants;

TEST_CASE("Add tickers") {
    ClientResponseLFQueue client_response_queue(MAX_MARKET_UPDATES);
    MarketUpdateLFQueue market_update_queue(MAX_MARKET_UPDATES);
    Exchange exchange(&client_response_queue, &market_update_queue);

    exchange.addTicker(0);
    exchange.addTicker(1);
}

TEST_CASE("Add order") {
    TickerIdT spy = 0;
    TickerIdT qqq = 1;
    ClientResponseLFQueue client_response_queue(MAX_MARKET_UPDATES);
    MarketUpdateLFQueue market_update_queue(MAX_MARKET_UPDATES);
    Exchange exchange(&client_response_queue, &market_update_queue, {spy, qqq});

    ClientIdT client_id = 2;
    QuantityT quantity = 5;
    PriceT price = 10;

    ClientResponse* update;

    exchange.addOrder(spy, client_id, OrderSide::BUY, quantity, price - 1);
    update = client_response_queue.pop();

    CHECK(update->type == ResponseType::ACCEPTED);
    CHECK(update->ticker_id == spy);
    CHECK(update->order_id == 1);
    CHECK(update->side == OrderSide::BUY);
    CHECK(update->quantity == quantity);
    CHECK(update->price == price - 1);
    CHECK(update->ns_timestamp != NsTimestamp_INVALID);

    update = client_response_queue.pop();

    CHECK(update == nullptr);

    exchange.addOrder(qqq, client_id, OrderSide::SELL, quantity + 1, price + 2);
    update = client_response_queue.pop();

    CHECK(update->type == ResponseType::ACCEPTED);
    CHECK(update->ticker_id == qqq);
    CHECK(update->order_id == 2);
    CHECK(update->side == OrderSide::SELL);
    CHECK(update->quantity == quantity + 1);
    CHECK(update->price == price + 2);
    CHECK(update->ns_timestamp != NsTimestamp_INVALID);
}

TEST_CASE("Exchange cancel order") {
    TickerIdT ticker_id = 0;
    ClientResponseLFQueue client_response_queue(MAX_MARKET_UPDATES);
    MarketUpdateLFQueue market_update_queue(MAX_MARKET_UPDATES);
    Exchange exchange(&client_response_queue, &market_update_queue, { ticker_id });

    ClientIdT client_id = 2;
    QuantityT quantity = 5;
    PriceT price = 10;

    ClientResponse* update;

    exchange.addOrder(ticker_id, client_id, OrderSide::BUY, quantity, price);
    update = client_response_queue.pop();
    auto add_ts = update->ns_timestamp;

    exchange.cancelOrder(ticker_id, client_id, update->order_id, OrderSide::BUY, price);

    update = client_response_queue.pop();

    CHECK(update->type == ResponseType::CANCELED);
    CHECK(update->ticker_id == ticker_id);
    CHECK(update->order_id == 1);
    CHECK(update->side == OrderSide::BUY);
    CHECK(update->quantity == quantity);
    CHECK(update->price == price);
    CHECK(update->ns_timestamp > add_ts);
}
