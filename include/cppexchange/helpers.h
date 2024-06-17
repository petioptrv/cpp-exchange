//
// Created by Petio Petrov on 2024-06-16.
//

#ifndef CPPEXCHANGE_HELPERS_H
#define CPPEXCHANGE_HELPERS_H

#include <string>

namespace helpers {
    std::string getenv_with_default(const std::string& variable_name,
                                    const std::string& default_value) {
        const char* value = getenv(variable_name.c_str());
        return value ? value : default_value;
    }

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
}  // namespace helpers

#endif  // CPPEXCHANGE_HELPERS_H
