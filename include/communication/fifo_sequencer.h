//
// Created by Petio Petrov on 2025-06-17.
//

#ifndef FIFO_SEQUENCER_H
#define FIFO_SEQUENCER_H
#include "cppexchange/messages.h"

namespace Communication {
    constexpr size_t MAX_PENDING_REQUESTS = 1024;

    struct ReceiveTimeClientRequest {
        Utils::NsTimestampT receive_timestamp;
        CPPExchange::ClientRequest request;

        auto operator<(const ReceiveTimeClientRequest &other) const {
            return receive_timestamp < other.receive_timestamp;
        }
    };

    class FifoSequencer {
    public:
        explicit FifoSequencer(
            CPPExchange::ClientRequestLFQueue *requests
        ) : requests(requests), pending_requests() {
        }

        auto addRequest(const CPPExchange::ClientRequest &request, const Utils::NsTimestampT timestamp) {
            if (UNLIKELY(pending_requests_index == MAX_PENDING_REQUESTS)) {
                FATAL("Too many pending requests.");
            }

            pending_requests.at(pending_requests_index++) = std::move(
                ReceiveTimeClientRequest{timestamp, request}
            );
        }

        auto sequenceAndPublish() {
            std::sort(pending_requests.begin(), pending_requests.begin() + pending_requests_index);

            for (int i = 0; i < pending_requests_index; i++) {
                requests->push(std::move(pending_requests[i].request));
            }

            pending_requests_index = 0;
        }

    private:
        CPPExchange::ClientRequestLFQueue *requests = nullptr;
        std::array<ReceiveTimeClientRequest, MAX_PENDING_REQUESTS> pending_requests;
        size_t pending_requests_index = 0;
    };
}

#endif //FIFO_SEQUENCER_H
