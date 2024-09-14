//
// Created by Petio Petrov on 2024-07-16.
//

#pragma once

#include <vector>


namespace Communication {
    /**
     * Lock-free queue
     */
    template <typename T> class LFQueue final {
      public:
        explicit LFQueue(std::size_t n) : data_(n, T()) {}

        bool empty() {
            return items_count_ == 0;
        }

        void push(T&& item) {
            data_[next_write_index_] = item;
            next_write_index_ = (next_write_index_ + 1) % data_.size();
            items_count_ = std::min(
                items_count_ + 1,
                data_.size()
            );
        }

        auto pop() {
            auto item = items_count_ ? &data_[next_read_index_] : nullptr;
            if (item != nullptr) {
                next_read_index_ = (next_read_index_ + 1) % data_.size();
                --items_count_;
            }
            return item;
        }

      private:
        std::vector<T> data_;
        std::atomic<size_t> next_write_index_ = {0};
        std::atomic<size_t> next_read_index_ = {0};
        std::atomic<size_t> items_count_ = {0};
    };
}  // namespace Communication
