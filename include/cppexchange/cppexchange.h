#pragma once

#include <string>
#include <unordered_map>

#include "cppexchange/constants.h"
#include "cppexchange/order.h"
#include "cppexchange/orderbook.h"
#include "cppexchange/updates.h"
#include "utils/macros.h"

namespace CPPExchange {
    /**
     * Exchange Class
     *
     * Currently, can only add tickers, but cannot remove.
     * The implementation uses an STD `unordered_map` to store the order books
     * without any object pool memory management since the number of tickers is very
     * limited and order books will be instantiated at startup and never destroyed.
     */
    class Exchange {
      private:
        OrderIdT next_order_id = 1;
        TradeIdT next_trade_id = 1;
        std::unordered_map<TickerIdT, CPPExchange::OrderBook*> order_books_map;
        ClientResponseLFQueue* client_response_queue;
        MarketUpdateLFQueue* market_update_queue;

        inline CPPExchange::OrderBook* getOrderBook(const TickerIdT ticker_id) {
            return order_books_map[ticker_id];
        }

        OrderIdT getNextOrderId() { return next_order_id++; }

      public:
        explicit Exchange(
            ClientResponseLFQueue* client_response_queue,
            MarketUpdateLFQueue* market_update_queue,
            const std::vector<const TickerIdT>& ticker_ids = {}
        )
            : order_books_map(Constants::MAX_TICKERS),
              client_response_queue(client_response_queue),
              market_update_queue(market_update_queue) {
            for (const auto& ticker_id : ticker_ids) {
                addTicker(ticker_id);
            }
        }
        Exchange(const Exchange& other) = delete;
        Exchange(const Exchange&& other) = delete;

        Exchange& operator=(Exchange& other) = delete;
        Exchange& operator=(Exchange&& other) = delete;

        void addTicker(const TickerIdT ticker_id) {
            ASSERT(order_books_map.size() != Constants::MAX_TICKERS, "Max tickers count reached");
            if (LIKELY(!order_books_map.contains(ticker_id))) {
                order_books_map[ticker_id] = new CPPExchange::OrderBook(
                    ticker_id, client_response_queue, market_update_queue
                );
            }
        }

        void addOrder(
            const TickerIdT ticker_id,
            ClientIdT client_id,
            Orders::OrderSide side,
            QuantityT quantity,
            PriceT limit_price
        ) {
            auto order_book = getOrderBook(ticker_id);
            order_book->add(getNextOrderId(), client_id, side, quantity, limit_price, next_trade_id);
        }

        void cancelOrder(
            const TickerIdT ticker_id,
            ClientIdT client_id,
            OrderIdT order_id,
            Orders::OrderSide side,
            PriceT limit_price
        ) {
            order_books_map[ticker_id]->cancel(client_id, order_id, side, limit_price);
        }
    };

}  // namespace CPPExchange
