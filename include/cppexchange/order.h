//
// Created by Petio Petrov on 2024-07-17.
//

#pragma once

#include <cstdint>

#include "cppexchange/typedefs.h"

namespace Orders {

    enum OrderSide { BUY, SELL };

    struct Order {
        OrderIdT order_id;
        ClientIdT client_id;
        MsTimestampT ms_timestamp;
        OrderSide side;
        QuantityT quantity;
        PriceT limit_price;
        QuantityT executed{0};
        
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
              ms_timestamp(ms_timestamp),
              side(side),
              quantity(quantity),
              limit_price(limit_price) {}
    };

    struct BuyOrderCompare {
        bool operator()(const Order* first, const Order* second) const {
            return (
                first->limit_price == second->limit_price
                    ? first->ms_timestamp > second->ms_timestamp
                    : first->limit_price < second->limit_price
            );
        }
    };

    struct SellOrderCompare {
        bool operator()(const Order* first, const Order* second) const {
            return (
                first->limit_price == second->limit_price
                    ? first->ms_timestamp > second->ms_timestamp
                    : first->limit_price > second->limit_price
            );
        }
    };

}  // namespace Orders
