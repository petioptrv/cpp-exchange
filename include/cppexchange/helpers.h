//
// Created by Petio Petrov on 2024-06-16.
//

#pragma once

#include <chrono>
#include <string>

#include "cppexchange/typedefs.h"

namespace Helpers {
    std::string
    getEnvWithDefault(const std::string& variable_name, const std::string& default_value);

    NsTimestampT getCurrentNsTimestamp();

    template <class T> class Singleton {
      public:
        Singleton& operator=(const Singleton&) = delete;
        Singleton& operator=(Singleton&&) = delete;

        static T& get_instance() {
            if (!instance) instance = new T_Instance;
            return *instance;
        }

      protected:
        Singleton() {}

      private:
        static inline T* instance = nullptr;
        struct T_Instance : public T {
            T_Instance() : T() {}
        };
    };
}  // namespace Helpers
