//
// Created by Petio Petrov on 2024-07-29.
//

#include "utils/object_pool.h"
#include "cppexchange/order.h"

#include <doctest/doctest.h>

using namespace Utils;
using namespace Orders;

TEST_CASE("Order object pool") {
    ObjectPool<Order> p(3);
    ClientIdT client_id = 0;

    Order* order_0 = p.getObject(0, client_id, 2, SELL, 1, 9);
    Order* order_1 = p.getObject(1, client_id, 2, SELL, 2, 9);

    CHECK(order_0 != order_1);

    p.releaseObject(order_0);
    p.releaseObject(order_1);

    p.getObject(2, client_id, 3, BUY, 2, 8);
    p.getObject(3, client_id, 3, SELL, 2, 8);
}
