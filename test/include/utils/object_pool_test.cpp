//
// Created by Petio Petrov on 2024-07-29.
//

#include "utils/object_pool.h"

#include <doctest/doctest.h>

#include "cppexchange/order.h"
#include "utils/time_utils.h"

using namespace Utils;
using namespace Orders;
using namespace Common;

TEST_CASE("Order object pool") {
    ObjectPool<Order> p(3);
    TickerIdT ticker_id = 0;
    ClientIdT client_id = 0;

    Order* order_0 = p.getObject(ticker_id, 0, client_id, getCurrentNsTimestamp(), OrderSide::SELL, 1, 9);
    Order* order_1 = p.getObject(ticker_id, 1, client_id, getCurrentNsTimestamp(), OrderSide::SELL, 2, 9);

    CHECK(order_0 != order_1);

    p.releaseObject(order_0);
    p.releaseObject(order_1);

    p.getObject(ticker_id, 2, client_id, getCurrentNsTimestamp(), OrderSide::BUY, 2, 8);
    p.getObject(ticker_id, 3, client_id, getCurrentNsTimestamp(), OrderSide::SELL, 2, 8);
}
