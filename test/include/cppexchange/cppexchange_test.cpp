//
// Created by Petio Petrov on 2024-08-30.
//

#include "cppexchange/cppexchange.h"

#include <doctest/doctest.h>

#include <thread>

#include "cppexchange/constants.h"
#include "cppexchange/order.h"

using namespace std;
using namespace CPPExchange;
using namespace Orders;
using namespace Constants;

TEST_CASE("Add tickers") {
    ClientRequestLFQueue client_request_queue(REQUESTS_QUEUE_SIZE);
    ClientResponseLFQueue client_response_queue(UPDATES_QUEUE_SIZE);
    MarketUpdateLFQueue market_update_queue(UPDATES_QUEUE_SIZE);
    Exchange exchange(&client_request_queue, &client_response_queue, &market_update_queue);

    exchange.addTicker(0);
    exchange.addTicker(1);
}

TEST_CASE("Add order") {
    TickerIdT spy = 0;
    TickerIdT qqq = 1;
    ClientRequestLFQueue client_request_queue(REQUESTS_QUEUE_SIZE);
    ClientResponseLFQueue client_response_queue(UPDATES_QUEUE_SIZE);
    MarketUpdateLFQueue market_update_queue(UPDATES_QUEUE_SIZE);
    Exchange exchange(
        &client_request_queue, &client_response_queue, &market_update_queue, {spy, qqq}
    );

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

TEST_CASE("Asynchronous order add") {
    TickerIdT ticker_id = 0;
    ClientRequestLFQueue client_request_queue(REQUESTS_QUEUE_SIZE);
    ClientResponseLFQueue client_response_queue(UPDATES_QUEUE_SIZE);
    MarketUpdateLFQueue market_update_queue(UPDATES_QUEUE_SIZE);
    Exchange exchange(
        &client_request_queue, &client_response_queue, &market_update_queue, {ticker_id}
    );

    exchange.start();

    ClientIdT client_id = 2;
    QuantityT quantity = 5;
    PriceT price = 10;

    client_request_queue.push(
        {RequestType::ADD, ticker_id, client_id, OrderId_INVALID, OrderSide::BUY, quantity, price}
    );

    for (int i = 0; i != 10 && client_response_queue.empty(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100 * i));
    }

    ClientResponse* update = client_response_queue.pop();

    CHECK(update->type == ResponseType::ACCEPTED);
    CHECK(update->ticker_id == ticker_id);
    CHECK(update->order_id == 1);
    CHECK(update->side == OrderSide::BUY);
    CHECK(update->quantity == quantity);
    CHECK(update->price == price);
    CHECK(update->ns_timestamp != NsTimestamp_INVALID);

    exchange.stop();
}

TEST_CASE("Exchange cancel order") {
    TickerIdT ticker_id = 0;
    ClientRequestLFQueue client_request_queue(REQUESTS_QUEUE_SIZE);
    ClientResponseLFQueue client_response_queue(UPDATES_QUEUE_SIZE);
    MarketUpdateLFQueue market_update_queue(UPDATES_QUEUE_SIZE);
    Exchange exchange(
        &client_request_queue, &client_response_queue, &market_update_queue, {ticker_id}
    );

    exchange.start();

    ClientIdT client_id = 2;
    QuantityT quantity = 5;
    PriceT price = 10;

    ClientResponse* update;

    exchange.addOrder(ticker_id, client_id, OrderSide::BUY, quantity, price);
    update = client_response_queue.pop();
    auto add_ts = update->ns_timestamp;

    client_request_queue.push(
        {RequestType::CANCEL, ticker_id, client_id, update->order_id, OrderSide::BUY, quantity, price}
    );

    for (int i = 0; i != 10 && client_response_queue.empty(); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100 * i));
    }

    update = client_response_queue.pop();

    CHECK(update->type == ResponseType::CANCELED);
    CHECK(update->ticker_id == ticker_id);
    CHECK(update->order_id == 1);
    CHECK(update->side == OrderSide::BUY);
    CHECK(update->quantity == quantity);
    CHECK(update->price == price);
    CHECK(update->ns_timestamp > add_ts);

    exchange.stop();
}
