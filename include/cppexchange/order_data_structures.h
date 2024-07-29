//
// Created by Petio Petrov on 2024-07-20.
//

#pragma once

#include <algorithm>
#include <deque>
#include <queue>
#include <vector>

#include "cppexchange/order.h"
#include "utils/singleton.h"

namespace OrderDataStructures {
    // todo: experiment with pre-allocating batches of consecutive orders

    typedef std::vector<Orders::Order*>::iterator OrdersIterator;

    template <class Compare = std::less<Orders::Order*>> class OrderQueue {
      public:
        OrderQueue() : comp() {}
        bool empty() { return orders.empty(); }
        void push(Orders::Order* order) {
            orders.push_back(order);
            std::push_heap(orders.begin(), orders.end(), comp);
        }
        Orders::Order* top() { return orders.front(); }
        void pop() {
            std::pop_heap(orders.begin(), orders.end(), comp);
            orders.pop_back();
        }
        OrdersIterator begin() {
            return orders.begin();
        }
        OrdersIterator end() {
            return orders.end();
        }
        void erase(OrdersIterator it) {
            orders.erase(it);
            std::make_heap(orders.begin(),orders.end());
        }

      private:
        std::vector<Orders::Order*> orders;
        Compare comp;
    };

    typedef OrderQueue<Orders::BuyOrderCompare> BuyOrderQueue;
    typedef OrderQueue<Orders::SellOrderCompare> SellOrderQueue;
}  // namespace OrderDataStructures
