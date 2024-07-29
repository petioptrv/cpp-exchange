//
// Created by Petio Petrov on 2024-07-17.
//

#pragma once

#include "cppexchange/config.h"
#include "cppexchange/order.h"
#include "cppexchange/order_data_structures.h"
#include "utils/object_pool.h"

class OrderBook final {
  public:
    OrderBook() : order_pool(Constants::ME_MAX_ORDER_IDS) {}
    std::vector<Orders::Order*> top(Orders::OrderSide side, int n_levels) {
        OrderDataStructures::OrdersIterator it_begin;
        OrderDataStructures::OrdersIterator it_end;
        std::vector<Orders::Order*> levels;

        if (side == Orders::BUY) {
            it_begin = bids.begin();
            it_end = bids.end();
        } else {
            it_begin = asks.begin();
            it_end = asks.end();
        }

        while (n_levels != 0 && it_begin != it_end) {
            levels.push_back((*it_begin));
            ++it_begin;
        }

        return levels;
    }

    bool add(
        ClientIdT client_id,
        MsTimestampT ms_timestamp,
        Orders::OrderSide side,
        QuantityT quantity,
        PriceT limit_price
    ) {
        if (side == Orders::BUY) {
            asks.push(order_pool.getObject(0, client_id, ms_timestamp, side, quantity, limit_price));
        } else {
            bids.push(order_pool.getObject(1, client_id, ms_timestamp, side, quantity, limit_price));
        }

        return true;
    }

    bool remove(OrderIdT order_id) {
        bool removed = false;
        auto bids_iter = bids.begin();
        auto asks_iter = asks.begin();

        while ((bids_iter != bids.end() || asks_iter != asks.end()) && !removed) {
            if (bids_iter != bids.end()) {
                if ((*bids_iter)->order_id == order_id) {
                    bids.erase(bids_iter);
                    removed = true;
                } else {
                    ++bids_iter;
                }
            }
            if (!removed && asks_iter != asks.end()) {
                if ((*asks_iter)->order_id == order_id) {
                    asks.erase(asks_iter);
                    removed = true;
                } else {
                    ++asks_iter;
                }
            }
        }

        return removed;
    }

  private:
    OrderDataStructures::BuyOrderQueue bids;
    OrderDataStructures::SellOrderQueue asks;

    Utils::ObjectPool<Orders::Order> order_pool;
};
