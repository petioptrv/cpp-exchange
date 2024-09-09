#pragma once

#include <string>
#include <unordered_map>

#include "cppexchange/constants.h"
#include "cppexchange/order.h"
#include "cppexchange/orderbook.h"
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
        std::unordered_map<TickerIdT, CPPExchange::OrderBook*> order_books_map;
        std::unordered_map<std::string, TickerIdT> tickers_map;

        inline CPPExchange::OrderBook* getOrderBook(const std::string& ticker) {
            auto ticker_id = getTickerId(ticker);
            return order_books_map[ticker_id];
        }

        inline std::size_t getTickerId(const std::string& ticker) {
            if (UNLIKELY(!tickers_map.contains(ticker))) {
                tickers_map[ticker] = tickers_map.size();
            }

            return tickers_map[ticker];
        }

      public:
        Exchange()
            : order_books_map(Constants::MAX_TICKERS), tickers_map(Constants::MAX_TICKERS) {};
        explicit Exchange(const std::vector<std::string>& tickers)
            : order_books_map(Constants::MAX_TICKERS), tickers_map(Constants::MAX_TICKERS) {
            for (const auto& ticker : tickers) {
                addTicker(ticker);
            }
        }
        Exchange(const Exchange& other) = delete;
        Exchange(const Exchange&& other) = delete;

        Exchange& operator=(Exchange& other) = delete;
        Exchange& operator=(Exchange&& other) = delete;

        void addTicker(const std::string& ticker) {
            ASSERT(order_books_map.size() != Constants::MAX_TICKERS, "Max tickers count reached");
            auto ticker_id = getTickerId(ticker);
            if (LIKELY(!order_books_map.contains(ticker_id))) {
                order_books_map[ticker_id] = new CPPExchange::OrderBook(ticker_id);
            }
        }

        Orders::Order* top(const std::string& ticker, Orders::OrderSide side) {
            auto order_book = getOrderBook(ticker);
            return order_book->top(side);
        }

        Orders::Order* addOrder(
            const std::string& ticker,
            ClientIdT client_id,
            Orders::OrderSide side,
            QuantityT quantity,
            PriceT limit_price
        ) {
            auto order_book = getOrderBook(ticker);
            return order_book->add(client_id, side, quantity, limit_price);
        }

        void removeOrder(Orders::Order* order) {
            order_books_map[order->ticker_id]->remove(order);
        }
    };

}  // namespace CPPExchange
