#pragma once

#include <string>
#include <thread>
#include <unordered_map>

#include "cppexchange/constants.h"
#include "cppexchange/messages.h"
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
        bool running = false;
        std::thread run_thread;

        OrderIdT next_order_id = 1;
        TradeIdT next_trade_id = 1;

        std::unordered_map<TickerIdT, CPPExchange::OrderBook*> order_books_map;

        ClientRequest* next_request;
        ClientRequestLFQueue* client_request_queue;
        ClientResponseLFQueue* client_response_queue;
        MarketUpdateLFQueue* market_update_queue;

        void run() noexcept {
            while (running) {
                next_request = client_request_queue->pop();
                if (LIKELY(next_request != nullptr)) {
                    processRequest();
                }
            }
        }

        void processRequest() {
            switch (next_request->type) {
                case RequestType::INVALID:
                    break;
                case RequestType::ADD:
                    addOrder(
                        next_request->ticker_id,
                        next_request->client_id,
                        next_request->side,
                        next_request->quantity,
                        next_request->limit_price
                    );
                    break;
                case RequestType::CANCEL:
                    cancelOrder(
                        next_request->ticker_id,
                        next_request->client_id,
                        next_request->order_id,
                        next_request->side,
                        next_request->limit_price
                    );
                    break;
            }
        }

        inline CPPExchange::OrderBook* getOrderBook(const TickerIdT ticker_id) {
            return order_books_map[ticker_id];
        }

        OrderIdT getNextOrderId() { return next_order_id++; }

      public:
        explicit Exchange(
            ClientRequestLFQueue* client_request_queue,
            ClientResponseLFQueue* client_response_queue,
            MarketUpdateLFQueue* market_update_queue,
            const std::vector<const TickerIdT>& ticker_ids = {}
        )
            : order_books_map(Constants::MAX_TICKERS),
              client_request_queue(client_request_queue),
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

        void start() {
            running = true;
            run_thread = std::thread([this]() { run(); });
        }

        void stop() {
            running = false;
            run_thread.join();
        }

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
            order_book->add(
                getNextOrderId(), client_id, side, quantity, limit_price, next_trade_id
            );
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
