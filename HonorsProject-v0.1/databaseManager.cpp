#include "databaseManager.h"
#include "moodEntry.h"
#include <sqlite3.h>
#include <iostream>

DatabaseManager::DatabaseManager() {
    int exit = sqlite3_open("vitality_hub.db", &db);
    
    if (exit != SQLITE_OK) {
        std::cerr << "Error opening database!" << std::endl;
    } else {
        // We build the "Islands" first, then the "Bridge"
        createEntryTable();      // 1. Mood Logs
        createTriggerListTable(); // 2. Master Trigger List
        createBridgeTable();      // 3. The Connector
    }
}

DatabaseManager::~DatabaseManager() {
    sqlite3_close(db);
}

// 1. THE MOOD LOGS
void DatabaseManager::createEntryTable() {
    // Removed the "trigger" column since it's now in the bridge!
    std::string sql = "CREATE TABLE IF NOT EXISTS mood_entry ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "created_at TEXT DEFAULT CURRENT_TIMESTAMP, "
                      "stress_level INTEGER, "
                      "anxiety_level INTEGER, "
                      "note TEXT"
                      ");";
    
    sqlite3_exec(db, sql.c_str(), NULL, 0, nullptr);
}

// 2. THE MASTER TRIGGER DICTIONARY
void DatabaseManager::createTriggerListTable() {
    // Added PRIMARY KEY and UNIQUE to prevent duplicate "Work" entries
    std::string sql = "CREATE TABLE IF NOT EXISTS trigger_list ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "name TEXT UNIQUE"
                      ");";
    
    sqlite3_exec(db, sql.c_str(), NULL, 0, nullptr);
}

// 3. THE BRIDGE (Connecting Moods to Triggers)
void DatabaseManager::createBridgeTable() {
    // References must match 'mood_entry' and 'trigger_list' exactly
    std::string sql = "CREATE TABLE IF NOT EXISTS mood_trigger_bridge ("
                      "entry_id INTEGER, "
                      "trigger_id INTEGER, "
                      "FOREIGN KEY(entry_id) REFERENCES mood_entry(id), "
                      "FOREIGN KEY(trigger_id) REFERENCES trigger_list(id)"
                      ");";
    
    sqlite3_exec(db, sql.c_str(), NULL, 0, nullptr);
}

// 4. CREATES AN ENTRY WHEN CALLED
void DatabaseManager::createEntry(const MoodEntry& entry) {
    char* errorMessage = nullptr;

    // Part A: Save the Mood
    std::string moodSql = "INSERT INTO mood_entry (stress_level, anxiety_level, note) VALUES (" +
                          std::to_string(entry.getStressLevel()) + ", " +
                          std::to_string(entry.getAnxietyLevel()) + ", " +
                          "'" + entry.getNote() + "');";

    int exit = sqlite3_exec(db, moodSql.c_str(), NULL, 0, &errorMessage);

    if (exit != SQLITE_OK) {
        std::cerr << "Mood Insert Error: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return;
    }
    
    long long newEntryId = sqlite3_last_insert_rowid(db);

    // Part B: The Trigger Loop
    for (const std::string& triggerName : entry.getTriggers()) {
        
        // 1. Add to master list (IGNORE if it's already there)
        std::string listSql = "INSERT OR IGNORE INTO trigger_list (name) VALUES ('" + triggerName + "');";
        sqlite3_exec(db, listSql.c_str(), NULL, 0, nullptr);

        // 2. Link them in the bridge
        std::string bridgeSql = "INSERT INTO mood_trigger_bridge (entry_id, trigger_id) VALUES (" +
                                std::to_string(newEntryId) + ", " +
                                "(SELECT id FROM trigger_list WHERE name = '" + triggerName + "'));";
        
        sqlite3_exec(db, bridgeSql.c_str(), NULL, 0, nullptr);
    }

    std::cout << "Log Saved Successfully! ✅" << std::endl;
}
