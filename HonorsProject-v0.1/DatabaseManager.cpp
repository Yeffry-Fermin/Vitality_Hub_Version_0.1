#include "DatabaseManager.h"
#include "MoodEntry.h"
#include <sqlite3.h>
#include <iostream>

// 0. Constructor to setup the tables
DatabaseManager::DatabaseManager() {
    int exit = sqlite3_open("vitality_hub.db", &db);
    
    if (exit != SQLITE_OK) {
        std::cerr << "Error opening database!" << std::endl;
    } else {
        createEntryTable();
        // createTriggerListTable(); // No longer needed
        // createBridgeTable();      // No longer needed
    }
}

DatabaseManager::~DatabaseManager() {
    sqlite3_close(db);
}

// 1. THE MOOD LOGS
void DatabaseManager::createEntryTable() {
    std::string sql = "CREATE TABLE IF NOT EXISTS mood_entry ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "created_at TEXT DEFAULT CURRENT_TIMESTAMP, "
                      "stress_level INTEGER, "
                      "anxiety_level INTEGER, "
                      "note TEXT, "
                      "triggers TEXT" // <--- Add this!
                      ");";
    
    sqlite3_exec(db, sql.c_str(), NULL, 0, nullptr);
}



// 4. Creates an entry and saves it to dabatase
void DatabaseManager::createEntry(const MoodEntry& entry) {
    char* errorMessage = nullptr;

    // --- STEP A: Squish the triggers into one string ---
    std::string triggerString = "";
    std::vector<std::string> triggers = entry.getTriggers();
    for (size_t i = 0; i < triggers.size(); ++i) {
        triggerString += triggers[i];
        if (i < triggers.size() - 1) triggerString += ", "; // Add comma between words
    }

    // --- STEP B: Update the SQL to include the 'triggers' column ---
    std::string moodSql = "INSERT INTO mood_entry (stress_level, anxiety_level, note, triggers) VALUES (" +
                          std::to_string(entry.getStressLevel()) + ", " +
                          std::to_string(entry.getAnxietyLevel()) + ", " +
                          "'" + entry.getNote() + "', " +
                          "'" + triggerString + "');";

    int exit = sqlite3_exec(db, moodSql.c_str(), NULL, 0, &errorMessage);

    if (exit != SQLITE_OK) {
        std::cerr << "Mood Insert Error: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return;
    }

    std::cout << "Log Saved Successfully!" << std::endl;
}

// Get all entries history

std::vector<MoodEntry> DatabaseManager::getEntries() {
    std::vector<MoodEntry> entries;
    std::string sql = "SELECT * FROM mood_entry;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        
        // Loop through every row the database finds
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            
            // 1. Extract values by column index (Order: id, timestamp, stress, anxiety, note, triggers)
            int id = sqlite3_column_int(stmt, 0);
            std::string timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            int stress = sqlite3_column_int(stmt, 2);
            int anxiety = sqlite3_column_int(stmt, 3);
            std::string note = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            std::string rawTriggers = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

            // 2. Use parser to turn the string back into a vector
            std::vector<std::string> triggerList = MoodEntry::parseTriggers(rawTriggers);
            // 3. Create the object and add it to our list
            // Constructor order: (id, stress, anxiety, note, triggers, timestamp)
            entries.emplace_back(id, stress, anxiety, note, triggerList, timestamp);
        }
    } else {
        std::cerr << "Fetch Error: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt); // Clean up the "bookmark"
    return entries;
}

//Bridge and JOin tables If i have time or future design

//// 2. THE MASTER TRIGGER DICTIONARY
//void DatabaseManager::createTriggerListTable() {
//    // Added PRIMARY KEY and UNIQUE to prevent duplicate "Work" entries
//    std::string sql = "CREATE TABLE IF NOT EXISTS trigger_list ("
//                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
//                      "name TEXT UNIQUE"
//                      ");";
//
//    sqlite3_exec(db, sql.c_str(), NULL, 0, nullptr);
//}

//// 3. THE BRIDGE (Connecting Moods to Triggers)
//void DatabaseManager::createBridgeTable() {
//    // References must match 'mood_entry' and 'trigger_list' exactly
//    std::string sql = "CREATE TABLE IF NOT EXISTS mood_trigger_bridge ("
//                      "entry_id INTEGER, "
//                      "trigger_id INTEGER, "
//                      "FOREIGN KEY(entry_id) REFERENCES mood_entry(id), "
//                      "FOREIGN KEY(trigger_id) REFERENCES trigger_list(id)"
//                      ");";
//
//    sqlite3_exec(db, sql.c_str(), NULL, 0, nullptr);
//}
