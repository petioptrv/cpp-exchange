//
// Created by Petio Petrov on 2024-07-17.
//

#include "cppexchange/order.h"
#include "cppexchange/order_data_structures.h"

#include <doctest/doctest.h>

using namespace Orders;
using namespace OrderDataStructures;

TEST_CASE("Order properties") {
    Order order(0, 1, 2, BUY, 4, 5);

    CHECK(order.order_id == 0);
    CHECK(order.client_id == 1);
    CHECK(order.side == BUY);
    CHECK(order.quantity == 4);
    CHECK(order.executed == 0);
}
