//
// Created by Petio Petrov on 2024-06-16.
//

#ifndef CPPEXCHANGE_POSTGRESQL_H
#define CPPEXCHANGE_POSTGRESQL_H

#include <pqxx/pqxx>

#include "db.h"

class PostgreSQL : public DB {
  public:
    bool is_connected() override;
    PostgreSQL();
    ~PostgreSQL() override;

  private:
    pqxx::connection connection;
};

class PostgreSQLBuilder : public DBBuilder {
  public:
    PostgreSQL* get_connection_instance() override;
};

#endif  // CPPEXCHANGE_POSTGRESQL_H
