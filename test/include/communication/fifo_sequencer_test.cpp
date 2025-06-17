//
// Created by Petio Petrov on 2025-06-17.
//

#include <doctest/doctest.h>

#include "communication/fifo_sequencer.h"

TEST_CASE("Sequencer sorts requests") {
    using namespace Communication;

    CPPExchange::ClientRequestLFQueue requests(10);
    FifoSequencer sequencer(&requests);

    sequencer.addRequest(
        CPPExchange::ClientRequest{
            CPPExchange::RequestType::ADD,
            0,
            1,
            2,
            Orders::OrderSide::BUY,
            10,
            5,
        },
        1
    );
    sequencer.addRequest(
        CPPExchange::ClientRequest{
            CPPExchange::RequestType::ADD,
            0,
            2,
            2,
            Orders::OrderSide::BUY,
            10,
            5,
        },
        3
    );
    sequencer.addRequest(
        CPPExchange::ClientRequest{
            CPPExchange::RequestType::ADD,
            0,
            3,
            2,
            Orders::OrderSide::BUY,
            10,
            5,
        },
        2
    );

    sequencer.sequenceAndPublish();

    CHECK(requests.pop()->client_id == 1);
    CHECK(requests.pop()->client_id == 3);
    CHECK(requests.pop()->client_id == 2);
}
