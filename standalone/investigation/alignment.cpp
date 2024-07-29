#include <cstdio>
#include <cstdint>
#include <cstddef>

typedef uint32_t OrderIdT;
typedef uint32_t ClientIdT;
typedef uint64_t MsTimestampT;
typedef float QuantityT;
typedef float PriceT;

namespace Orders {

    enum OrderSide { BUY, SELL };

    struct Order {
        OrderIdT order_id;
        ClientIdT client_id;
        MsTimestampT ms_timestamp;
        QuantityT quantity;
        PriceT limit_price;
        QuantityT executed{0};
        OrderSide side;

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
              quantity(quantity),
              limit_price(limit_price),
              side(side) {}
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

}

int main() {
    printf("Order offsetof oid:%lu cid:%lu mst:%lu q:%lu lp:%lu e:%lu s:%lu size:%lu",
           offsetof(struct Orders::Order, order_id), offsetof(struct Orders::Order, client_id),
           offsetof(struct Orders::Order, ms_timestamp), offsetof(struct Orders::Order, quantity),
           offsetof(struct Orders::Order, limit_price), offsetof(struct Orders::Order, executed),
           offsetof(struct Orders::Order, side), sizeof(Orders::Order));
    printf("\nOrder alignof oid:%zu cid:%zu mst:%zu q:%zu lp:%zu e:%zu s:%zu",
           alignof(OrderIdT), alignof(ClientIdT), alignof(MsTimestampT), alignof( QuantityT),
           alignof(PriceT), alignof( QuantityT), alignof(Orders::OrderSide));
}