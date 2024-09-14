//
// Created by Petio Petrov on 2024-07-17.
//

#pragma once

#include <cstdint>

#include "cppexchange/typedefs.h"
#include "utils/macros.h"

namespace Orders {

    enum class OrderSide : uint8_t { INVALID = 0, BUY = 1, SELL = 2 };

    struct Order {
        Order* previous = nullptr;
        Order* next = nullptr;

        TickerIdT ticker_id = TickerId_INVALID;
        OrderIdT order_id = OrderId_INVALID;
        ClientIdT client_id = ClientId_INVALID;
        QuantityT quantity = Quantity_INVALID;
        QuantityT executed{0};
        NsTimestampT ns_timestamp = NsTimestamp_INVALID;
        PriceT limit_price = Price_INVALID;
        OrderSide side = OrderSide::INVALID;

        Order() = default;

        Order(
            TickerIdT ticker_id,
            OrderIdT order_id,
            ClientIdT client_id,
            NsTimestampT ns_timestamp,
            OrderSide side,
            QuantityT quantity,
            PriceT limit_price
        )
            : ticker_id(ticker_id),
              order_id(order_id),
              client_id(client_id),
              quantity(quantity),
              ns_timestamp(ns_timestamp),
              limit_price(limit_price),
              side(side) {}
    };

    class OrdersPriceLevel {
      public:
        OrdersPriceLevel() = default;

        OrdersPriceLevel(OrderSide side, PriceT price, OrdersPriceLevel* next_price_level)
            : next_price_level(next_price_level), price_level(price), side(side) {}

        void setPriceLevel(PriceT price) {
            ASSERT(
                top_order == nullptr, "Attempting to change the price level for existing orders."
            );
            price_level = price;
        }

        [[nodiscard]] PriceT getPrice() const { return price_level; }

        void setNextPriceLevel(OrdersPriceLevel* new_next_price_level) {
            next_price_level = new_next_price_level;
        }

        OrdersPriceLevel* getNextPriceLevel() { return next_price_level; }

        [[maybe_unused]] [[nodiscard]] OrderSide getSide() const { return side; }

        void add(Orders::Order* order) {
            if (UNLIKELY(top_order == nullptr)) {
                top_order = order;
                order->previous = order->next = order;
                side = order->side;
            } else {
                order->next = top_order;
                order->previous = top_order->previous;
                order->previous->next = order;
                top_order->previous = order;
            }
        }

        Orders::Order* cancel(OrderIdT order_id) {
            auto order = top_order;

            while (order->order_id != order_id && order != top_order->previous) {
                order = order->next;
            }

            if (order->order_id == order_id) {
                order->previous->next = order->next;
                order->next->previous = order->previous;
                if (UNLIKELY(top_order == order)) {
                    if (UNLIKELY(top_order->next == top_order)) {
                        top_order = nullptr;
                    } else {
                        top_order = top_order->next;
                    }
                }
                order->next = order->previous = nullptr;
            } else {
                order = nullptr;
            }

            return order;
        }

        Orders::Order* top() { return top_order; }

      private:
        OrdersPriceLevel* next_price_level = nullptr;
        PriceT price_level = Price_INVALID;
        Order* top_order = nullptr;
        OrderSide side = OrderSide::INVALID;
    };

}  // namespace Orders
