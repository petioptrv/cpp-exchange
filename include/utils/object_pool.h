//
// Created by Petio Petrov on 2024-07-29.
//

#pragma once

#include <utils/singleton.h>

#include <vector>

#include "macros.h"

namespace Utils {
    template<typename T>
    class ObjectPool final {
      public:
        explicit ObjectPool(const size_t size) : store(size, {T(), true}) {}

        template<typename... Args>
        T* getObject(Args... args) noexcept {
            auto block = &(store[next_free]);
            T* object = &(block->object);

            ASSERT(block->available, "Expected free ObjectBlock at index: " + std::to_string(next_free));
            object = new(object) T(args...);
            block->available = false;

            updateNextFree();

            return object;
        }

        auto releaseObject(const T* object) noexcept {
            const auto object_index = (reinterpret_cast<const ObjectBlock*>(object) - &store[0]);

            ASSERT(object_index >= 0 && static_cast<size_t>(object_index) < store.size(),
                   "Element being freed does not belong to this object pool.");
            ASSERT(!store[object_index].available, "Attempting to free an already available ObjectBlock.");

            store[object_index].available = true;
        }

        // Deleted default, copy & move constructors and assignment-operators.
        ObjectPool() = delete;

        ObjectPool(const ObjectPool &) = delete;

        ObjectPool(const ObjectPool &&) = delete;

        ObjectPool &operator=(const ObjectPool &) = delete;

        ObjectPool &operator=(const ObjectPool &&) = delete;

      private:
        struct ObjectBlock {
            T object;
            bool available = true;
        };

        std::vector<ObjectBlock> store;
        size_t next_free = 0;

        void updateNextFree() {
            const auto prev_free = next_free;

            while (!store[next_free].available) {
                ++next_free;
                if (UNLIKELY(next_free == store.size())) {
                    next_free = 0;
                }
                if (UNLIKELY(next_free == prev_free)) {
                    ASSERT(next_free != prev_free, "Buffer is full.");
                }
            }
        }
    };
}

