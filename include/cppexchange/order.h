//
// Created by Petio Petrov on 2024-07-17.
//

#pragma once

#include <cstdint>

#include "cppexchange/typedefs.h"
#include "utils/macros.h"

namespace Orders {

    enum OrderSide { BUY, SELL, INVALID };

    struct Order {
        Order* previous = nullptr;
        Order* next = nullptr;

        OrderIdT order_id = OrderId_INVALID;
        ClientIdT client_id = ClientId_INVALID;
        QuantityT quantity = Quantity_INVALID;
        QuantityT executed{0};
        MsTimestampT ms_timestamp = MsTimestamp_INVALID;
        PriceT limit_price = Price_INVALID;
        OrderSide side = INVALID;

        Order() = default;

        Order(
            OrderIdT order_id,
            ClientIdT client_id,
            MsTimestampT ms_timestamp,
            OrderSide side,
            QuantityT quantity,
            PriceT limit_price
        )
            : order_id(order_id),
              client_id(client_id),
              quantity(quantity),
              ms_timestamp(ms_timestamp),
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

        [[maybe_unused]] OrderSide getSide() const { return side; }

        void add(Orders::Order* order) {
            ASSERT(
                order->limit_price == price_level, "Attempting to add an order to the wrong level."
            );
            if (UNLIKELY(top_order == nullptr)) {
                top_order = order;
                order->previous = order->next = order;
                side = order->side;
            } else {
                order->next = top_order->previous;
                top_order->previous->next = order;
                top_order->previous = order;
            }
        }

        void remove(Orders::Order* order) {
            ASSERT(
                order->limit_price == price_level,
                "Attempting to remove an order from the wrong level."
            );
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
        }

        Orders::Order* top() { return top_order; }

      private:
        OrdersPriceLevel* next_price_level = nullptr;
        PriceT price_level = Price_INVALID;
        Order* top_order = nullptr;
        OrderSide side = INVALID;
    };

}  // namespace Orders
