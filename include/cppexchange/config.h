//
// Created by Petio Petrov on 2024-06-16.
//

#ifndef CPPEXCHANGE_CONFIG_H
#define CPPEXCHANGE_CONFIG_H

#include <string>

#include "constants.h"
#include "dotenv.h"
#include "helpers.h"

namespace config {
    class Config : public Helpers::Singleton<Config> {
      public:
        [[nodiscard]] const std::string& get_postgres_server() const { return POSTGRES_SERVER; }
        [[nodiscard]] const int& get_postgres_port() const { return POSTGRES_PORT; }
        [[nodiscard]] const std::string& get_postgres_db() const { return POSTGRES_DB; }
        [[nodiscard]] const std::string& get_postgres_user() const { return POSTGRES_USER; }
        [[nodiscard]] const std::string& get_postgres_password() const { return POSTGRES_PASSWORD; }

      protected:
        Config() {
            dotenv::init((Constants::PROJECT_ROOT / ".env").string().c_str());

            POSTGRES_SERVER = Helpers::getEnvWithDefault("POSTGRES_SERVER", "localhost");
            POSTGRES_PORT = stoi(Helpers::getEnvWithDefault("POSTGRES_PORT", "5432"));
            POSTGRES_DB = Helpers::getEnvWithDefault("POSTGRES_DB", "app");
            POSTGRES_USER = Helpers::getEnvWithDefault("POSTGRES_USER", "postgres");
            POSTGRES_PASSWORD = Helpers::getEnvWithDefault("POSTGRES_PASSWORD", "admin");
        }

      private:
        // postgres DB
        std::string POSTGRES_SERVER;
        int POSTGRES_PORT;
        std::string POSTGRES_DB;
        std::string POSTGRES_USER;
        std::string POSTGRES_PASSWORD;
    };
}  // namespace config

#endif  // CPPEXCHANGE_CONFIG_H
