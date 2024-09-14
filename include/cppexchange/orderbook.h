//
// Created by Petio Petrov on 2024-07-17.
//

#pragma once

#include "cppexchange/config.h"
#include "cppexchange/constants.h"
#include "cppexchange/helpers.h"
#include "cppexchange/order.h"
#include "cppexchange/updates.h"
#include "utils/object_pool.h"

namespace CPPExchange {
    class OrderBook final {
      public:
        explicit OrderBook(
            const TickerIdT ticker_id,
            ClientResponseLFQueue* client_response_queue,
            MarketUpdateLFQueue* market_update_queue
        )
            : ticker_id(ticker_id),
              order_pool(Constants::MAX_ORDER_IDS),
              price_level_pool(Constants::MAX_PRICE_LEVELS),
              client_response_queue(client_response_queue),
              market_update_queue(market_update_queue) {}

        void add(
            OrderIdT order_id,
            ClientIdT client_id,
            Orders::OrderSide side,
            QuantityT quantity,
            PriceT limit_price,
            TradeIdT& next_trade_id
        ) {
            client_response_queue->push(
                {ResponseType::ACCEPTED,
                 ticker_id,
                 client_id,
                 order_id,
                 side,
                 quantity,
                 limit_price,
                 Helpers::getCurrentNsTimestamp()}
            );

            QuantityT remaining
                = checkForMatch(order_id, client_id, side, quantity, limit_price, next_trade_id);

            if (remaining != 0) {
                auto price_level = getPriceLevel(limit_price, side, true);
                auto order = order_pool.getObject(
                    ticker_id,
                    order_id,
                    client_id,
                    Helpers::getCurrentNsTimestamp(),
                    side,
                    remaining,
                    limit_price
                );

                price_level->add(order);
                market_update_queue->push(
                    {UpdateType::ADD,
                     ticker_id,
                     order_id,
                     TradeId_INVALID,
                     side,
                     remaining,
                     limit_price,
                     Helpers::getCurrentNsTimestamp()}
                );
            }
        }

        void
        cancel(ClientIdT client_id, OrderIdT order_id, Orders::OrderSide side, PriceT limit_price) {
            Orders::Order* order;
            auto price_level = getPriceLevel(limit_price, side);

            if (LIKELY(price_level != nullptr)) {
                order = price_level->cancel(order_id);
                if (price_level->top() == nullptr) {
                    removePriceLevel(price_level);
                }
            } else {
                order = nullptr;
            }

            if (order != nullptr) {
                auto update_ns = Helpers::getCurrentNsTimestamp();
                client_response_queue->push(
                    {ResponseType::CANCELED,
                     ticker_id,
                     client_id,
                     order_id,
                     side,
                     order->quantity,
                     limit_price,
                     update_ns}
                );
                market_update_queue->push(
                    {UpdateType::REMOVE,
                     ticker_id,
                     order_id,
                     TradeId_INVALID,
                     side,
                     order->quantity,
                     limit_price,
                     update_ns}
                );
            } else {
                client_response_queue->push(
                    {ResponseType::CANCEL_REJECTED,
                     ticker_id,
                     client_id,
                     order_id,
                     Orders::OrderSide::INVALID,
                     Quantity_INVALID,
                     Price_INVALID,
                     Helpers::getCurrentNsTimestamp()}
                );
            }
        }

      private:
        TickerIdT ticker_id;
        Utils::ObjectPool<Orders::Order> order_pool;
        Utils::ObjectPool<Orders::OrdersPriceLevel> price_level_pool;
        std::array<Orders::OrdersPriceLevel*, Constants::MAX_PRICE_LEVELS> price_levels{};
        Orders::OrdersPriceLevel* best_bids = nullptr;
        Orders::OrdersPriceLevel* best_asks = nullptr;

        ClientResponseLFQueue* client_response_queue;
        MarketUpdateLFQueue* market_update_queue;

        QuantityT checkForMatch(
            OrderIdT order_id,
            ClientIdT client_id,
            Orders::OrderSide side,
            QuantityT quantity,
            PriceT limit_price,
            TradeIdT& next_trade_id
        ) {
            Orders::Order* opposite_order;
            if (side == Orders::OrderSide::BUY) {
                while (quantity != 0 && best_asks != nullptr && best_asks->getPrice() <= limit_price
                ) {
                    opposite_order = best_asks->top();
                    quantity
                        = match(order_id, client_id, side, quantity, next_trade_id, opposite_order);
                    if (best_asks->top() == nullptr) {
                        removePriceLevel(best_asks);
                    }
                }
            } else {
                while (quantity != 0 && best_bids != nullptr && best_bids->getPrice() >= limit_price
                ) {
                    opposite_order = best_bids->top();
                    quantity
                        = match(order_id, client_id, side, quantity, next_trade_id, opposite_order);
                    if (best_bids->top() == nullptr) {
                        removePriceLevel(best_bids);
                    }
                }
            }

            return quantity;
        }

        QuantityT match(
            OrderIdT order_id,
            ClientIdT client_id,
            Orders::OrderSide side,
            QuantityT quantity,
            TradeIdT& next_trade_id,
            Orders::Order* opposite_order
        ) {
            QuantityT fill_quantity = std::min(quantity, opposite_order->quantity);

            market_update_queue->push(
                {UpdateType::TRADE,
                 ticker_id,
                 OrderId_INVALID,
                 next_trade_id++,
                 side,
                 fill_quantity,
                 opposite_order->limit_price,
                 Helpers::getCurrentNsTimestamp()}
            );

            opposite_order->quantity -= fill_quantity;

            if (opposite_order->quantity == 0) {
                market_update_queue->push(
                    {UpdateType::REMOVE,
                     ticker_id,
                     opposite_order->order_id,
                     TradeId_INVALID,
                     opposite_order->side,
                     fill_quantity,
                     opposite_order->limit_price,
                     Helpers::getCurrentNsTimestamp()}
                );
                getPriceLevel(opposite_order->limit_price, side)->cancel(opposite_order->order_id);
            } else {
                market_update_queue->push(
                    {UpdateType::MODIFY,
                     ticker_id,
                     opposite_order->order_id,
                     TradeId_INVALID,
                     opposite_order->side,
                     opposite_order->quantity,
                     opposite_order->limit_price,
                     Helpers::getCurrentNsTimestamp()}
                );
            }

            client_response_queue->push(
                {ResponseType::FILLED,
                 ticker_id,
                 opposite_order->client_id,
                 opposite_order->order_id,
                 opposite_order->side,
                 fill_quantity,
                 opposite_order->limit_price,
                 Helpers::getCurrentNsTimestamp()}
            );
            client_response_queue->push(
                {ResponseType::FILLED,
                 ticker_id,
                 client_id,
                 order_id,
                 side,
                 fill_quantity,
                 opposite_order->limit_price,
                 Helpers::getCurrentNsTimestamp()}
            );

            quantity -= fill_quantity;

            return quantity;
        }

        Orders::OrdersPriceLevel*
        getPriceLevel(PriceT price, Orders::OrderSide side, bool create_if_missing = false) {
            auto price_level_index = getPriceLevelIndex(price);
            auto price_level = price_levels[price_level_index];

            if (UNLIKELY(price_level == nullptr && create_if_missing)) {
                price_level = insertPriceLevel(price, side, price_level_index);
            }

            return price_level;
        }

        Orders::OrdersPriceLevel*
        insertPriceLevel(PriceT price, Orders::OrderSide side, size_t price_level_index) {
            Orders::OrdersPriceLevel* next_price_level;
            Orders::OrdersPriceLevel* best_price_level;

            if (side == Orders::OrderSide::BUY) {
                best_price_level = best_bids;
                next_price_level = best_bids;
                while (LIKELY(next_price_level != nullptr) && next_price_level->getPrice() > price
                ) {
                    next_price_level = next_price_level->getNextPriceLevel();
                }
            } else {
                best_price_level = best_asks;
                next_price_level = best_asks;
                while (LIKELY(next_price_level != nullptr) && next_price_level->getPrice() < price
                ) {
                    next_price_level = next_price_level->getNextPriceLevel();
                }
            }

            auto price_level = price_level_pool.getObject(side, price, next_price_level);
            price_levels[price_level_index] = price_level;

            if (UNLIKELY(next_price_level == best_price_level)) {
                (side == Orders::OrderSide::BUY ? best_bids : best_asks) = price_level;
            }

            return price_level;
        }

        void removePriceLevel(Orders::OrdersPriceLevel* price_level) {
            Orders::OrdersPriceLevel* prev_price_level = nullptr;

            if (price_level->getSide() == Orders::OrderSide::BUY) {
                if (price_level != best_bids) {
                    prev_price_level = best_bids;
                }
            } else {
                if (price_level != best_asks) {
                    prev_price_level = best_asks;
                }
            }
            if (LIKELY(prev_price_level != nullptr)) {
                while (prev_price_level->getNextPriceLevel()->getPrice() != price_level->getPrice()
                ) {
                    prev_price_level = prev_price_level->getNextPriceLevel();
                }
            }

            if (UNLIKELY(prev_price_level == nullptr)) {
                (price_level->getSide() == Orders::OrderSide::BUY ? best_bids : best_asks)
                    = price_level->getNextPriceLevel();
            } else {
                prev_price_level->setNextPriceLevel(price_level->getNextPriceLevel());
            }

            auto price_level_index = getPriceLevelIndex(price_level->getPrice());
            price_levels[price_level_index] = nullptr;

            price_level_pool.releaseObject(price_level);
        }

        static inline size_t getPriceLevelIndex(PriceT price) {
            return price % Constants::MAX_PRICE_LEVELS;
        }
    };

}  // namespace CPPExchange