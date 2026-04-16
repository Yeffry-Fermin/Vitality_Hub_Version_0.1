#pragma once
#include <sqlite3.h>

class DatabaseManager {
private:
    sqlite3 *db;

public:
    void createTable();
    void createEntry();
    void
    DatabaseManager();
    ~DatabaseManager();
};
