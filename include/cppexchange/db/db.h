//
// Created by Petio Petrov on 2024-06-16.
//

#ifndef CPPEXCHANGE_DB_H
#define CPPEXCHANGE_DB_H

#include <exception>

class DBConnectionError : public std::exception {};

class DB {
  public:
    virtual bool is_connected() = 0;
    virtual ~DB() = default;
};

class DBBuilder {
  public:
    virtual DB* get_connection_instance() = 0;
};

#endif  // CPPEXCHANGE_DB_H
