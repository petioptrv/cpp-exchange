//
// Created by Petio Petrov on 2024-07-20.
//

#pragma once

namespace Utils {
    template <class T> class Singleton {
      public:
        Singleton& operator=(const Singleton& other) = delete;
        Singleton& operator=(Singleton&& other) = delete;
        static T& getInstance() {
            if (instance == nullptr) {
                instance = new TInstance();
            }
            return *instance;
        }

      protected:
        Singleton() {}

      private:
        struct TInstance : public T {
            TInstance() : T() {}
        };

        static inline T* instance = nullptr;
    };
}  // namespace Utils
