//
// Created by Petio Petrov on 2024-06-16.
//

#include <fmt/format.h>
#include <iostream>

#include "cppexchange/db/db.h"
#include "cppexchange/db/postgresql.h"
#include "cppexchange/config.h"

PostgreSQL::PostgreSQL() {
    auto& conf = config::Config::get_instance();
    std::string connection_string = fmt::format(
        "dbname = {}"
        " user = {}"
        " password = {}"
        " host = {}"
        " port = {}",
        conf.get_postgres_db(), conf.get_postgres_user(), conf.get_postgres_password(),
        conf.get_postgres_server(), std::to_string(conf.get_postgres_port()));

    std::cout << "in here" << std::endl;
    std::cout << connection_string << std::endl;

    connection = pqxx::connection(connection_string);

    if (!connection.is_open()) {
        throw DBConnectionError();
    }
}

 PostgreSQL::~PostgreSQL() {
    if (connection.is_open()) {
        connection.close();
    }
}

bool PostgreSQL::is_connected() { return connection.is_open(); }

PostgreSQL* PostgreSQLBuilder::get_connection_instance() { return new PostgreSQL(); }
