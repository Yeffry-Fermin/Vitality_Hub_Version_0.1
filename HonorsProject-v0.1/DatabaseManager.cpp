#include "DatabaseManager.h"
#include "MoodEntry.h"
#include "./external/sqlite/sqlite3.h"
#include <iostream>

// Constructor opens database and ensures tables exist
DatabaseManager::DatabaseManager() {
    int exit = sqlite3_open("vitality_hub.db", &db);
    
    if (exit != SQLITE_OK) {
        std::cerr << "Error opening database!" << std::endl;
    } else {
        createEntryTable();
    }
}

DatabaseManager::~DatabaseManager() {
    sqlite3_close(db);
}

// Creates the mood entry table if it does not already exist
void DatabaseManager::createEntryTable() {
    std::string sql = "CREATE TABLE IF NOT EXISTS mood_entry ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
    "created_at TEXT DEFAULT CURRENT_TIMESTAMP, "
    "stress_level INTEGER, "
    "energy_level INTEGER, "
    "sleep_hours REAL, "
    "note TEXT, "
    "triggers TEXT"
    ");";
    
    sqlite3_exec(db, sql.c_str(), NULL, 0, nullptr);
}

// Saves a new mood entry into the database
void DatabaseManager::createEntry(const MoodEntry& entry) {
    char* errorMessage = nullptr;
    
    // Convert trigger vector into one comma-separated string
    std::string triggerString = "";
    std::vector<std::string> triggers = entry.getTriggers();
    
    for (size_t i = 0; i < triggers.size(); ++i) {
        triggerString += triggers[i];
        
        if (i < triggers.size() - 1)
            triggerString += ", ";
    }
    
    // Build SQL insert statement
    std::string moodSql =
    "INSERT INTO mood_entry "
    "(stress_level, energy_level, sleep_hours, note, triggers) VALUES (" +
    std::to_string(entry.getStressLevel()) + ", " +
    std::to_string(entry.getEnergyLevel()) + ", " +
    std::to_string(entry.getSleepHours()) + ", " +
    "'" + entry.getNote() + "', " +
    "'" + triggerString + "');";
    
    int exit = sqlite3_exec(db, moodSql.c_str(), NULL, 0, &errorMessage);
    
    // Handle SQL insertion errors
    if (exit != SQLITE_OK) {
        std::cerr << "Mood Insert Error: " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
        return;
    }
    
    std::cout << "Log Saved Successfully!" << std::endl;
}

// Fetch mood entries from database
// If days > 0, only recent entries are returned
std::vector<MoodEntry> DatabaseManager::getEntries(int days) {
    
    std::vector<MoodEntry> entries;
    std::string sql;
    
    // Decide which SQL query to run
    if (days > 0) {
        sql = "SELECT * FROM mood_entry WHERE created_at >= DATETIME('now', ?);";
    } else {
        sql = "SELECT * FROM mood_entry;";
    }
    
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        
        // Bind dynamic day filter into query
        if (days > 0) {
            std::string dayParam = "-" + std::to_string(days) + " days";
            
            sqlite3_bind_text(
                              stmt,
                              1,
                              dayParam.c_str(),
                              -1,
                              SQLITE_TRANSIENT);
        }
        
        int rowCount = 0;
        
        // Read each SQL row
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            
            int id = sqlite3_column_int(stmt, 0);
            rowCount++;
            
            // Safely convert SQLite text into std::string
            auto toStr = [](const unsigned char* text) { // this is basically a lambda function(small inline helper function)
                return text
                ? std::string(reinterpret_cast<const char*>(text))
                : "";
            };
            
            std::string timestamp = toStr(sqlite3_column_text(stmt, 1));
            
            // Extract columns from SQL row
            int stress = sqlite3_column_int(stmt, 2);
            int energy = sqlite3_column_int(stmt, 3);
            double sleep = sqlite3_column_double(stmt, 4);
            
            std::string note = toStr(sqlite3_column_text(stmt, 5));
            
            std::string rawTriggers = toStr(sqlite3_column_text(stmt, 6));
            
            // Convert trigger string back into vector
            std::vector<std::string> triggerList = MoodEntry::parseTriggers(rawTriggers);
            
            // Create MoodEntry object from SQL data
            entries.emplace_back(id,stress,energy,sleep,note,triggerList,timestamp);
        }
        
    } else {
        
        std::cerr << "Fetch Error: "
        << sqlite3_errmsg(db)
        << std::endl;
    }
    
    sqlite3_finalize(stmt);
    
    return entries;
}

// Generates rolling averages used for momentum tracking
std::vector<MomentumPoint> DatabaseManager::getMoodMomentum() {
    
    std::vector<MomentumPoint> points;
    
    // Rolling averages over the last 7 entries
    std::string sql =
    "SELECT created_at, "
    "AVG(stress_level) OVER (ORDER BY created_at ROWS BETWEEN 6 PRECEDING AND CURRENT ROW), "
    "AVG(energy_level) OVER (ORDER BY created_at ROWS BETWEEN 6 PRECEDING AND CURRENT ROW), "
    "AVG(sleep_hours) OVER (ORDER BY created_at ROWS BETWEEN 6 PRECEDING AND CURRENT ROW) "
    "FROM mood_entry "
    "WHERE created_at >= date('now', '-14 days') "
    "ORDER BY created_at DESC;";
    
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            
            MomentumPoint p;
            
            const unsigned char* dateText =
            sqlite3_column_text(stmt, 0);
            
            p.date = dateText
            ? reinterpret_cast<const char*>(dateText)
            : "Unknown";
            
            // Pull rolling averages from SQL result
            p.rollingStress =
            static_cast<float>(sqlite3_column_double(stmt, 1));
            
            p.rollingEnergy =
            static_cast<float>(sqlite3_column_double(stmt, 2));
            
            p.rollingSleep =
            static_cast<float>(sqlite3_column_double(stmt, 3));
            
            points.push_back(p);
        }
        
    } else {
        
        std::cerr << "Momentum Query Error: "
        << sqlite3_errmsg(db)
        << std::endl;
    }
    
    sqlite3_finalize(stmt);
    
    return points;
}
