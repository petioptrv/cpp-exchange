//
// Created by Petio Petrov on 2024-07-17.
//

#pragma once

#include "cppexchange/config.h"
#include "cppexchange/constants.h"
#include "cppexchange/helpers.h"
#include "cppexchange/order.h"
#include "utils/object_pool.h"

class OrderBook final {
    // The OrderBook is an efficient ledger for orders. It does not implement order-matching logic.
  public:
    OrderBook()
        : order_pool(Constants::MAX_ORDER_IDS),
          price_level_pool(Constants::MAX_PRICE_LEVELS),
          price_levels(Constants::MAX_PRICE_LEVELS, nullptr) {}

    Orders::Order*
    add(ClientIdT client_id, Orders::OrderSide side, QuantityT quantity, PriceT limit_price) {
        ASSERT(
            (side == Orders::OrderSide::BUY
             && (UNLIKELY(best_asks == nullptr) || best_asks->getPrice() > limit_price))
                || (side == Orders::OrderSide::SELL
                    && ((UNLIKELY(best_bids == nullptr) || best_bids->getPrice() < limit_price))),
            "Attempting to add an order to the wrong side of the order book"
        );

        auto price_level = getPriceLevel(limit_price, side);

        auto order = order_pool.getObject(
            getNextOrderId(),
            client_id,
            Helpers::getCurrentMsTimestamp(),
            side,
            quantity,
            limit_price
        );

        price_level->add(order);

        return order;
    }

    void remove(Orders::Order* order) {
        auto price_level = getPriceLevel(order->limit_price, order->side);
        price_level->remove(order);
        if (price_level->top() == nullptr) {
            removePriceLevel(price_level);
        }
    }

    Orders::Order* top(Orders::OrderSide side) {
        Orders::Order* order;

        if (side == Orders::OrderSide::BUY) {
            order = LIKELY(best_bids != nullptr) ? best_bids->top() : nullptr;
        } else {
            order = LIKELY(best_asks != nullptr) ? best_asks->top() : nullptr;
        }

        return order;
    }

  private:
    OrderIdT next_order_id = 1;
    Utils::ObjectPool<Orders::Order> order_pool;
    Utils::ObjectPool<Orders::OrdersPriceLevel> price_level_pool;
    std::vector<Orders::OrdersPriceLevel*> price_levels;
    Orders::OrdersPriceLevel* best_bids = nullptr;
    Orders::OrdersPriceLevel* best_asks = nullptr;

    OrderIdT getNextOrderId() { return next_order_id++; }

    Orders::OrdersPriceLevel* getPriceLevel(PriceT price, Orders::OrderSide side) {
        auto price_level_index = getPriceLevelIndex(price);
        auto price_level = price_levels[price_level_index];

        if (UNLIKELY(price_level == nullptr)) {
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
            while (LIKELY(next_price_level != nullptr) && next_price_level->getPrice() > price) {
                next_price_level = next_price_level->getNextPriceLevel();
            }
        } else {
            best_price_level = best_asks;
            next_price_level = best_asks;
            while (LIKELY(next_price_level != nullptr) && next_price_level->getPrice() < price) {
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
            while (prev_price_level->getNextPriceLevel()->getPrice() != price_level->getPrice()) {
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
