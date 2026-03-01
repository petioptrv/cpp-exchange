//
// Created by Petio Petrov on 2024-09-09.
//

#pragma once

#include "communication/lf_queue.h"
#include "cppexchange/order.h"
#include "typedefs.h"

namespace CPPExchange {
#pragma pack(push, 1)

    enum class RequestType: uint8_t {
        INVALID = 0,
        ADD = 1,
        CANCEL = 2
    };

    struct ClientRequest {
        // todo: add request ID to match response to request
        RequestType type = RequestType::INVALID;
        TickerIdT ticker_id = TickerId_INVALID;
        ClientIdT client_id = ClientId_INVALID;
        OrderIdT order_id = OrderId_INVALID;
        Orders::OrderSide side = Orders::OrderSide::INVALID;
        QuantityT quantity = Quantity_INVALID;
        PriceT limit_price = Price_INVALID;
    };

    struct MDPClientRequest {
        size_t sequence_number = 0;
        ClientRequest client_request;
    };

    enum class ResponseType : uint8_t {
        INVALID = 0,
        ACCEPTED = 1,
        FILLED = 2,
        CANCELED = 3,
        CANCEL_REJECTED = 4,
        ERROR = 5
    };

    struct ClientResponse {
        ResponseType type = ResponseType::INVALID;
        TickerIdT ticker_id = TickerId_INVALID;
        ClientIdT client_id = ClientId_INVALID;
        OrderIdT order_id = OrderId_INVALID;
        Orders::OrderSide side = Orders::OrderSide::INVALID;
        QuantityT quantity = Quantity_INVALID;
        PriceT price = Price_INVALID;
        Utils::NsTimestampT ns_timestamp = NsTimestamp_INVALID;
    };

    struct MDPClientResponse {
        size_t sequence_number = 0;
        ClientResponse *client_response;
    };

    enum class UpdateType : uint8_t { INVALID = 0, ADD = 1, MODIFY = 2, REMOVE = 3, TRADE = 4 };

    struct MarketUpdate {
        UpdateType type = UpdateType::INVALID;
        TickerIdT ticker_id = TickerId_INVALID;
        OrderIdT order_id = OrderId_INVALID;
        TradeIdT trade_id = TradeId_INVALID;
        Orders::OrderSide side = Orders::OrderSide::INVALID;
        QuantityT quantity = Quantity_INVALID;
        PriceT price = Price_INVALID;
        Utils::NsTimestampT ns_timestamp = NsTimestamp_INVALID;
    };

    struct MDPMarketUpdate {
        size_t sequence_number = 0;
        MarketUpdate market_update;
    };

#pragma pack(pop)

    typedef Communication::LFQueue<ClientRequest> ClientRequestLFQueue;
    typedef Communication::LFQueue<ClientResponse> ClientResponseLFQueue;
    typedef Communication::LFQueue<MarketUpdate> MarketUpdateLFQueue;
    typedef Communication::LFQueue<MDPMarketUpdate> MDPMarketUpdateLFQueue;
} // namespace CPPExchange
