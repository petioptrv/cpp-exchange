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
    class Config : public helpers::Singleton<Config> {
      public:
        [[nodiscard]] const std::string& get_postgres_server() const { return POSTGRES_SERVER; }
        [[nodiscard]] const int& get_postgres_port() const { return POSTGRES_PORT; }
        [[nodiscard]] const std::string& get_postgres_db() const { return POSTGRES_DB; }
        [[nodiscard]] const std::string& get_postgres_user() const { return POSTGRES_USER; }
        [[nodiscard]] const std::string& get_postgres_password() const { return POSTGRES_PASSWORD; }

      protected:
        Config() {
            dotenv::init((Constants::PROJECT_ROOT / ".env").string().c_str());

            POSTGRES_SERVER = helpers::getenv_with_default("POSTGRES_SERVER", "localhost");
            POSTGRES_PORT = stoi(helpers::getenv_with_default("POSTGRES_PORT", "5432"));
            POSTGRES_DB = helpers::getenv_with_default("POSTGRES_DB", "app");
            POSTGRES_USER = helpers::getenv_with_default("POSTGRES_USER", "postgres");
            POSTGRES_PASSWORD = helpers::getenv_with_default("POSTGRES_PASSWORD", "admin");
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
