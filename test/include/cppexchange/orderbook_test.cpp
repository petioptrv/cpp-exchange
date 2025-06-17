//
// Created by Petio Petrov on 2024-07-22.
//

#include <doctest/doctest.h>

#include "cppexchange/orderbook.h"
#include "cppexchange/constants.h"
#include "cppexchange/messages.h"
#include "cppexchange/order.h"
#include "cppexchange/typedefs.h"

using namespace Orders;
using namespace CPPExchange;
using namespace Constants;

TEST_CASE("Insert one bid and one ask maker orders") {
    TickerIdT ticker_id = 0;
    ClientResponseLFQueue client_response_queue(UPDATES_QUEUE_SIZE);
    MarketUpdateLFQueue market_update_queue(UPDATES_QUEUE_SIZE);
    OrderBook order_book(ticker_id, &client_response_queue, &market_update_queue);

    OrderIdT order_id = 0;
    ClientIdT client_id = 2;
    QuantityT quantity = 5;
    PriceT price = 10;
    TradeIdT next_trade_id = 1;

    ClientResponse* client_response;
    MarketUpdate* market_update;

    order_book.add(order_id++, client_id, OrderSide::BUY, quantity, price - 1, next_trade_id);
    client_response = client_response_queue.pop();

    CHECK(client_response->type == ResponseType::ACCEPTED);
    CHECK(client_response->ticker_id == ticker_id);
    CHECK(client_response->client_id == client_id);
    CHECK(client_response->order_id == order_id - 1);
    CHECK(client_response->side == OrderSide::BUY);
    CHECK(client_response->quantity == quantity);
    CHECK(client_response->price == price - 1);
    CHECK(client_response->ns_timestamp != NsTimestamp_INVALID);

    market_update = market_update_queue.pop();

    CHECK(market_update->type == UpdateType::ADD);
    CHECK(market_update->ticker_id == ticker_id);
    CHECK(client_response->client_id == client_id);
    CHECK(market_update->side == OrderSide::BUY);
    CHECK(client_response->quantity == quantity);
    CHECK(client_response->price == price - 1);
    CHECK(client_response->ns_timestamp != NsTimestamp_INVALID);

    order_book.add(order_id++, client_id, OrderSide::SELL, quantity + 1, price + 1, next_trade_id);
    client_response = client_response_queue.pop();

    CHECK(client_response->type == ResponseType::ACCEPTED);
    CHECK(client_response->ticker_id == ticker_id);
    CHECK(client_response->client_id == client_id);
    CHECK(client_response->order_id == order_id - 1);
    CHECK(client_response->side == OrderSide::SELL);
    CHECK(client_response->quantity == quantity + 1);
    CHECK(client_response->price == price + 1);
    CHECK(client_response->ns_timestamp != NsTimestamp_INVALID);

    market_update = market_update_queue.pop();

    CHECK(market_update->type == UpdateType::ADD);
    CHECK(market_update->ticker_id == ticker_id);
    CHECK(client_response->client_id == client_id);
    CHECK(market_update->side == OrderSide::SELL);
    CHECK(client_response->quantity == quantity + 1);
    CHECK(client_response->price == price + 1);
    CHECK(client_response->ns_timestamp != NsTimestamp_INVALID);
}

TEST_CASE("Orderbook cancel order") {
    TickerIdT ticker_id = 0;
    ClientResponseLFQueue client_response_queue(UPDATES_QUEUE_SIZE);
    MarketUpdateLFQueue market_updates_queue(UPDATES_QUEUE_SIZE);
    OrderBook order_book(ticker_id, &client_response_queue, &market_updates_queue);

    OrderIdT order_id = 0;
    ClientIdT client_id = 2;
    QuantityT quantity = 5;
    PriceT price = 10;
    TradeIdT next_trade_id = 1;

    ClientResponse* client_response;
    MarketUpdate* market_update;

    order_book.add(order_id, client_id, OrderSide::BUY, quantity, price, next_trade_id);
    client_response = client_response_queue.pop();
    auto add_ts = client_response->ns_timestamp;
    market_updates_queue.pop();

    order_book.cancel(client_id, order_id, OrderSide::BUY, price);

    client_response = client_response_queue.pop();

    CHECK(client_response->type == ResponseType::CANCELED);
    CHECK(client_response->ticker_id == ticker_id);
    CHECK(client_response->client_id == client_id);
    CHECK(client_response->order_id == order_id);
    CHECK(client_response->side == OrderSide::BUY);
    CHECK(client_response->quantity == quantity);
    CHECK(client_response->price == price);
    CHECK(client_response->ns_timestamp > add_ts);

    market_update = market_updates_queue.pop();

    CHECK(market_update->type == UpdateType::REMOVE);
    CHECK(market_update->ticker_id == ticker_id);
    CHECK(market_update->order_id == order_id);
    CHECK(market_update->trade_id == TradeId_INVALID);
    CHECK(market_update->side == OrderSide::BUY);
    CHECK(client_response->quantity == quantity);
    CHECK(client_response->price == price);
    CHECK(client_response->ns_timestamp != NsTimestamp_INVALID);
}

TEST_CASE("Add ask taker order, fully filled by single maker") {
    TickerIdT ticker_id = 0;
    OrderIdT maker_order_id = 1;
    OrderIdT taker_order_id = 2;
    ClientIdT maker_client_id = 1;
    ClientIdT taker_client_id = 2;
    QuantityT quantity = 1;
    PriceT ask_price = 10;
    TradeIdT next_trade_id = 1;

    ClientResponseLFQueue client_response_queue(UPDATES_QUEUE_SIZE);
    MarketUpdateLFQueue market_updates_queue(UPDATES_QUEUE_SIZE);
    OrderBook order_book(ticker_id, &client_response_queue, &market_updates_queue);

    ClientResponse* client_response;
    MarketUpdate* market_update;

    order_book.add(
        maker_order_id, maker_client_id, OrderSide::SELL, quantity, ask_price, next_trade_id
    );
    client_response_queue.pop(); // maker accepted
    market_updates_queue.pop();  // maker added

    order_book.add(
        taker_order_id, taker_client_id, OrderSide::BUY, quantity, ask_price + 1, next_trade_id
    );
    client_response = client_response_queue.pop(); // taker accepted

    CHECK(client_response->type == ResponseType::ACCEPTED);
    CHECK(client_response->order_id == taker_order_id);

    market_update = market_updates_queue.pop(); // trade

    CHECK(market_update->type == UpdateType::TRADE);
    CHECK(market_update->ticker_id == ticker_id);
    CHECK(market_update->order_id == OrderId_INVALID);
    CHECK(market_update->trade_id == next_trade_id - 1);
    CHECK(market_update->side == OrderSide::BUY); // the taker side
    CHECK(market_update->quantity == quantity);
    CHECK(market_update->price == ask_price); // maker price
    CHECK(market_update->ns_timestamp != NsTimestamp_INVALID);

    market_update = market_updates_queue.pop(); // remove maker order

    CHECK(market_update->type == UpdateType::REMOVE);
    CHECK(market_update->ticker_id == ticker_id);
    CHECK(market_update->order_id == maker_order_id);
    CHECK(market_update->trade_id == TickerId_INVALID);
    CHECK(market_update->side == OrderSide::SELL);
    CHECK(market_update->quantity == quantity);
    CHECK(market_update->price == ask_price);
    CHECK(market_update->ns_timestamp != NsTimestamp_INVALID);

    client_response = client_response_queue.pop(); // maker order filled

    CHECK(client_response->type == ResponseType::FILLED);
    CHECK(client_response->ticker_id == ticker_id);
    CHECK(client_response->client_id == maker_client_id);
    CHECK(client_response->order_id == maker_order_id);
    CHECK(client_response->side == OrderSide::SELL);
    CHECK(client_response->quantity == quantity);
    CHECK(client_response->price == ask_price);
    CHECK(client_response->ns_timestamp != NsTimestamp_INVALID);

    client_response = client_response_queue.pop(); // taker order filled

    CHECK(client_response->type == ResponseType::FILLED);
    CHECK(client_response->ticker_id == ticker_id);
    CHECK(client_response->client_id == taker_client_id);
    CHECK(client_response->order_id == taker_order_id);
    CHECK(client_response->side == OrderSide::BUY);
    CHECK(client_response->quantity == quantity);
    CHECK(client_response->price == ask_price);
    CHECK(client_response->ns_timestamp != NsTimestamp_INVALID);
}

TEST_CASE("Add ask taker order, fully filled by multiple makers") {
    /*
     * ASKS
     * Price Size
     *
     * 10    1
     * 11    1 + 2
     *
     * Taker should fill the first level and the first and half of the second orders
     * on the second level.
     */

    TickerIdT ticker_id = 0;

    OrderIdT first_maker_order_id = 1;
    OrderIdT second_maker_order_id = 2;
    OrderIdT third_maker_order_id = 3;
    OrderIdT taker_order_id = 4;

    ClientIdT first_maker_client_id = 1;
    ClientIdT second_maker_client_id = 2;
    ClientIdT third_maker_client_id = 3;
    ClientIdT taker_client_id = 4;

    QuantityT first_maker_quantity = 1;
    QuantityT second_maker_quantity = 1;
    QuantityT third_maker_quantity = 2;
    QuantityT taker_quantity = 3;

    PriceT first_level_ask_price = 10;  // for second maker
    PriceT second_level_ask_price = 11; // for first and third makers

    TradeIdT next_trade_id = 1;

    ClientResponseLFQueue client_response_queue(UPDATES_QUEUE_SIZE);
    MarketUpdateLFQueue market_updates_queue(UPDATES_QUEUE_SIZE);
    OrderBook order_book(ticker_id, &client_response_queue, &market_updates_queue);

    ClientResponse* client_response;
    MarketUpdate* market_update;

    order_book.add(
        first_maker_order_id,
        first_maker_client_id,
        OrderSide::SELL,
        first_maker_quantity,
        second_level_ask_price,
        next_trade_id
    );
    client_response_queue.pop(); // first maker accepted
    market_updates_queue.pop();  // first maker added

    order_book.add(
        second_maker_order_id,
        second_maker_client_id,
        OrderSide::SELL,
        second_maker_quantity,
        first_level_ask_price,
        next_trade_id
    );
    client_response_queue.pop(); // second maker accepted
    market_updates_queue.pop();  // second maker added

    order_book.add(
        third_maker_order_id,
        third_maker_client_id,
        OrderSide::SELL,
        third_maker_quantity,
        second_level_ask_price,
        next_trade_id
    );
    client_response_queue.pop(); // third maker accepted
    market_updates_queue.pop();  // third maker added

    order_book.add(
        taker_order_id,
        taker_client_id,
        OrderSide::BUY,
        taker_quantity,
        second_level_ask_price,
        next_trade_id
    );
    client_response_queue.pop(); // taker accepted

    market_update = market_updates_queue.pop(); // trade — second maker filled

    CHECK(market_update->type == UpdateType::TRADE);
    CHECK(market_update->trade_id == 1);
    CHECK(market_update->side == OrderSide::BUY); // the taker side
    CHECK(market_update->quantity == second_maker_quantity);
    CHECK(market_update->price == first_level_ask_price); // maker price

    market_update = market_updates_queue.pop(); // remove second maker order

    CHECK(market_update->type == UpdateType::REMOVE);
    CHECK(market_update->order_id == second_maker_order_id);

    client_response = client_response_queue.pop(); // second maker order filled

    CHECK(client_response->type == ResponseType::FILLED);
    CHECK(client_response->order_id == second_maker_order_id);

    client_response = client_response_queue.pop(); // taker order filled

    CHECK(client_response->type == ResponseType::FILLED);
    CHECK(client_response->order_id == taker_order_id);
    CHECK(client_response->quantity == second_maker_quantity);

    market_update = market_updates_queue.pop(); // trade — first maker filled

    CHECK(market_update->type == UpdateType::TRADE);
    CHECK(market_update->trade_id == 2);
    CHECK(market_update->side == OrderSide::BUY); // the taker side
    CHECK(market_update->quantity == first_maker_quantity);
    CHECK(market_update->price == second_level_ask_price); // maker price

    market_update = market_updates_queue.pop(); // remove first maker order

    CHECK(market_update->type == UpdateType::REMOVE);
    CHECK(market_update->order_id == first_maker_order_id);

    client_response = client_response_queue.pop(); // first maker order filled

    CHECK(client_response->type == ResponseType::FILLED);
    CHECK(client_response->order_id == first_maker_order_id);

    client_response = client_response_queue.pop(); // taker order filled

    CHECK(client_response->type == ResponseType::FILLED);
    CHECK(client_response->order_id == taker_order_id);
    CHECK(client_response->quantity == first_maker_quantity);

    market_update = market_updates_queue.pop(); // trade — third maker filled

    CHECK(market_update->type == UpdateType::TRADE);
    CHECK(market_update->trade_id == 3);
    CHECK(market_update->side == OrderSide::BUY); // the taker side
    CHECK(market_update->quantity == third_maker_quantity - 1);
    CHECK(market_update->price == second_level_ask_price); // maker price

    market_update = market_updates_queue.pop(); // remove third maker order

    CHECK(market_update->type == UpdateType::MODIFY);
    CHECK(market_update->order_id == third_maker_order_id);
    CHECK(market_update->trade_id == TickerId_INVALID);
    CHECK(market_update->side == OrderSide::SELL);
    CHECK(market_update->quantity == third_maker_quantity - 1);
    CHECK(market_update->price == second_level_ask_price);
    CHECK(market_update->ns_timestamp != NsTimestamp_INVALID);

    client_response = client_response_queue.pop(); // third maker order filled

    CHECK(client_response->type == ResponseType::FILLED);
    CHECK(client_response->order_id == third_maker_order_id);

    client_response = client_response_queue.pop(); // taker order filled

    CHECK(client_response->type == ResponseType::FILLED);
    CHECK(client_response->order_id == taker_order_id);
    CHECK(client_response->quantity == third_maker_quantity - 1);
}

TEST_CASE("Price wrap-around") {
    /*
     * ASKS
     * Price                    Size
     *
     * 10 + MAX_PRICE_LEVELS    1
     * 10                       1
     */

    TickerIdT ticker_id = 0;

    OrderIdT next_order_id = 1;
    OrderIdT first_maker_order_id = next_order_id++;
    OrderIdT second_maker_order_id = next_order_id++;
    OrderIdT taker_order_id = next_order_id++;

    ClientIdT next_client_id = 1;
    ClientIdT first_maker_client_id = next_client_id++;
    ClientIdT second_maker_client_id = next_client_id++;
    ClientIdT taker_client_id = next_client_id++;

    QuantityT first_maker_quantity = 1;
    QuantityT second_maker_quantity = 1;
    QuantityT taker_quantity = 1;

    PriceT faulty_ask_price = 10 + MAX_PRICE_LEVELS; // loop around
    PriceT good_ask_price = 10;

    TradeIdT next_trade_id = 1;

    ClientResponseLFQueue client_response_queue(UPDATES_QUEUE_SIZE);
    MarketUpdateLFQueue market_updates_queue(UPDATES_QUEUE_SIZE);
    OrderBook order_book(ticker_id, &client_response_queue, &market_updates_queue);

    ClientResponse* client_response;
    MarketUpdate* market_update;

    order_book.add(
        first_maker_order_id,
        first_maker_client_id,
        OrderSide::SELL,
        first_maker_quantity,
        faulty_ask_price,
        next_trade_id
    );
    client_response_queue.pop(); // first maker accepted
    market_updates_queue.pop();  // first maker added

    order_book.add(
        second_maker_order_id,
        second_maker_client_id,
        OrderSide::SELL,
        second_maker_quantity,
        good_ask_price,
        next_trade_id
    );
    client_response_queue.pop(); // second maker accepted
    market_updates_queue.pop();  // second maker added

    order_book.add(
        taker_order_id,
        taker_client_id,
        OrderSide::BUY,
        taker_quantity,
        good_ask_price,
        next_trade_id
    );
    client_response_queue.pop(); // taker accepted

    market_update = market_updates_queue.pop(); // trade — second maker filled

    CHECK(market_update->type == UpdateType::TRADE);
    CHECK(market_update->trade_id == 1);
    CHECK(market_update->side == OrderSide::BUY); // the taker side
    CHECK(market_update->quantity == second_maker_quantity);
    CHECK(market_update->price == good_ask_price); // maker price

    market_update = market_updates_queue.pop(); // remove second maker order

    CHECK(market_update->type == UpdateType::REMOVE);
    CHECK(market_update->order_id == second_maker_order_id);

    client_response = client_response_queue.pop(); // second maker order filled

    CHECK(client_response->type == ResponseType::FILLED);
    CHECK(client_response->client_id == second_maker_client_id);
    CHECK(client_response->order_id == second_maker_order_id);

    client_response = client_response_queue.pop(); // taker order filled

    CHECK(client_response->type == ResponseType::FILLED);
    CHECK(client_response->order_id == taker_order_id);
    CHECK(client_response->quantity == second_maker_quantity);
}

TEST_CASE("Add ask taker order, partially filled") {
    TickerIdT ticker_id = 0;
    OrderIdT maker_order_id = 1;
    OrderIdT taker_order_id = 2;
    ClientIdT maker_client_id = 1;
    ClientIdT taker_client_id = 2;
    QuantityT maker_quantity = 1;
    PriceT ask_price = 10;
    TradeIdT next_trade_id = 1;

    ClientResponseLFQueue client_response_queue(UPDATES_QUEUE_SIZE);
    MarketUpdateLFQueue market_updates_queue(UPDATES_QUEUE_SIZE);
    OrderBook order_book(ticker_id, &client_response_queue, &market_updates_queue);

    ClientResponse* client_response;
    MarketUpdate* market_update;

    order_book.add(
        maker_order_id, maker_client_id, OrderSide::SELL, maker_quantity, ask_price, next_trade_id
    );
    client_response_queue.pop(); // maker accepted
    market_updates_queue.pop();  // maker added

    order_book.add(
        taker_order_id,
        taker_client_id,
        OrderSide::BUY,
        maker_quantity + 1,
        ask_price + 1,
        next_trade_id
    );
    client_response = client_response_queue.pop(); // taker accepted

    CHECK(client_response->type == ResponseType::ACCEPTED);
    CHECK(client_response->order_id == taker_order_id);

    market_update = market_updates_queue.pop(); // trade

    CHECK(market_update->type == UpdateType::TRADE);
    CHECK(market_update->quantity == maker_quantity);

    market_update = market_updates_queue.pop(); // remove maker order

    CHECK(market_update->type == UpdateType::REMOVE);
    CHECK(market_update->order_id == maker_order_id);

    client_response = client_response_queue.pop(); // maker order filled

    CHECK(client_response->type == ResponseType::FILLED);
    CHECK(client_response->order_id == maker_order_id);
    CHECK(client_response->quantity == maker_quantity);

    client_response = client_response_queue.pop(); // taker order filled

    CHECK(client_response->type == ResponseType::FILLED);
    CHECK(client_response->order_id == taker_order_id);
    CHECK(client_response->quantity == maker_quantity);
    
    market_update = market_updates_queue.pop(); // unfilled taker is placed on book

    CHECK(market_update->type == UpdateType::ADD);
    CHECK(market_update->ticker_id == ticker_id);
    CHECK(market_update->side == OrderSide::BUY);
    CHECK(market_update->quantity == 1);
    CHECK(market_update->price == ask_price + 1);
}
