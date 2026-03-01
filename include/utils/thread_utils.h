//
// Created by Petio Petrov on 2025-06-17.
//

#ifndef THREAD_UTILS_H
#define THREAD_UTILS_H
#include <string>

namespace Utils {
    template<typename T, typename... A>
    inline auto createAndStartThread(int core_id, const std::string &name, T &&func, A &&... args) noexcept {
        auto t = new std::thread([&]() {
            // if (core_id >= 0 && !setThreadCore(core_id)) {
                // std::cerr << "Failed to set core affinity for " << name << " " << pthread_self() << " to " << core_id <<
                        // std::endl;
                // exit(EXIT_FAILURE);
            // }
            // std::cerr << "Set core affinity for " << name << " " << pthread_self() << " to " << core_id << std::endl;

            std::forward<T>(func)((std::forward<A>(args))...);
        });

        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(1s);

        return t;
    }
}

#endif //THREAD_UTILS_H
