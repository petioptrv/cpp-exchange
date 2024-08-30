#include <cstddef>
#include <cstdio>

#include "cppexchange/order.h"

void checkOrderMemoryAlignment() {
    printf(
        "\nOrder alignof  p:%zu n:%zu oid:%zu cid:%zu q:%zu e:%zu mst:%zu lp:%zu s:%zu",
        alignof(Orders::Order*),
        alignof(Orders::Order*),
        alignof(OrderIdT),
        alignof(ClientIdT),
        alignof(QuantityT),
        alignof(QuantityT),
        alignof(MsTimestampT),
        alignof(PriceT),
        alignof(Orders::OrderSide)
    );
    printf(
        "\nOrder sizeof   p:%lu n:%lu oid:%lu cid:%lu q:%lu e:%lu mst:%lu lp:%lu s:%lu size:%lu",
        sizeof(Orders::Order*),
        sizeof(Orders::Order*),
        sizeof(OrderIdT),
        sizeof(ClientIdT),
        sizeof(QuantityT),
        sizeof(QuantityT),
        sizeof(MsTimestampT),
        sizeof(PriceT),
        sizeof(Orders::OrderSide),
        sizeof(Orders::Order)
    );
    printf(
        "\nOrder offsetof p:%zu n:%zu oid:%zu cid:%zu q:%zu e:%zu mst:%zu lp:%zu s:%zu",
        offsetof(struct Orders::Order, previous),
        offsetof(struct Orders::Order, next),
        offsetof(struct Orders::Order, order_id),
        offsetof(struct Orders::Order, client_id),
        offsetof(struct Orders::Order, quantity),
        offsetof(struct Orders::Order, executed),
        offsetof(struct Orders::Order, ms_timestamp),
        offsetof(struct Orders::Order, limit_price),
        offsetof(struct Orders::Order, side)
    );
}

void checkOrdersPriceLevelMemoryAlignment() {
    printf(
        "\nOrdersPriceLevel alignof  npl:%zu pl:%zu s:%zu to:%zu",
        alignof(Orders::OrdersPriceLevel*),
        alignof(PriceT),
        alignof(Orders::Order*),
        alignof(Orders::OrderSide)
    );
    printf(
        "\nOrdersPriceLevel sizeof   npl:%lu pl:%lu s:%lu to:%lu size:%lu",
        sizeof(Orders::OrdersPriceLevel*),
        sizeof(PriceT),
        sizeof(Orders::Order*),
        sizeof(Orders::OrderSide),
        sizeof(Orders::OrdersPriceLevel)
    );
    printf(
        "\nOrdersPriceLevel offsetof npl:%zu pl:%zu s:%zu to:%zu",
        offsetof(Orders::OrdersPriceLevel, next_price_level),
        offsetof(Orders::OrdersPriceLevel, price_level),
        offsetof(Orders::OrdersPriceLevel, top_order),
        offsetof(Orders::OrdersPriceLevel, side)
    );
}

int main() {
//    checkOrderMemoryAlignment();
    checkOrdersPriceLevelMemoryAlignment();
}