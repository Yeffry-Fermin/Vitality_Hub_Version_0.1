#include "databaseManager.h"
#include <sqlite3.h>
#include <iostream>

DatabaseManager::DatabaseManager() {
    // 1. Open the database
    int exit = sqlite3_open("vitality_hub.db", &db);
    
    //check for status
    if (exit != SQLITE_OK) {
        // Something went wrong!
        std::cout << "Error opening database!" << std::endl;
    } else {
        std::cout << "Database opened successfully!" << std::endl;
    }
}

DatabaseManager::~DatabaseManager() {
    // close database
    sqlite3_close(db);
}

void DatabaseManager::createTable() {
    // 1. The SQL instruction (The message)
    std::string sql = "CREATE TABLE IF NOT EXISTS mood_entries ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
    "created_at TEXT DEFAULT CURRENT_TIMESTAMP, "
    "stress_level INTEGER, "
    "anxiety_level INTEGER, "
    "note TEXT, "
    "trigger VARCHAR()"
    ");";
    
    char* errorMessage = nullptr;
    
    // 2. Sending the messenger
    int exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &errorMessage);
    
    if (exit != SQLITE_OK) {
        std::cerr << "Error creating table: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    } else {
        std::cout << "Table ready for logs! ✅" << std::endl;
    }
}

void DatabaseManager::createEntry() {
    std::string sql_Entry = "INSERT INTO mood_entries (stress_level, anxiety_level, note, triggers) "
    "VALUES (7, 4, 'Had a busy morning', 'Work');";
    
    char* errorMessage = nullptr;
    
    int exit = sqlite3_exec(db, sql_Entry.c_str(), NULL, 0, &errorMessage);
    
    if(exit != SQLITE_OK) {
        std::cerr << "Error inserting data: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    } else {
        std::cout << "Log saved successfully! 💾" << std::endl;
    }
}
