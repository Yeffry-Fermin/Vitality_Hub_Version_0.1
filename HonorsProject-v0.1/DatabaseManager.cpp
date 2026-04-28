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

// Get entries by
std::vector<MoodEntry> DatabaseManager::getEntries(int days) {
    std::vector<MoodEntry> entries;
    std::string sql;
    
    // 1. Decide which query to use
    if (days > 0) {
        // Filtered Query
        sql = "SELECT * FROM mood_entry WHERE created_at >= DATETIME('now', ?);";
    } else {
        // All Time Query (No WHERE clause, no placeholder)
        sql = "SELECT * FROM mood_entry;";
    }

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        
        // 2. Only bind if we have a placeholder '?'
        if (days > 0) {
            std::string dayParam = "-" + std::to_string(days) + " days";
            sqlite3_bind_text(stmt, 1, dayParam.c_str(), -1, SQLITE_TRANSIENT);
        }
        int rowCount = 0; // Temporary counter
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            rowCount++;
            
            // 3. Safety Check: Handle potential null values from the database
            auto toStr = [](const unsigned char* text) {
                return text ? std::string(reinterpret_cast<const char*>(text)) : "";
            };

            std::string timestamp = toStr(sqlite3_column_text(stmt, 1));
            
           
            int stress = sqlite3_column_int(stmt, 2);
            int anxiety = sqlite3_column_int(stmt, 3);
            std::string note = toStr(sqlite3_column_text(stmt, 4));
            std::string rawTriggers = toStr(sqlite3_column_text(stmt, 5));

            std::vector<std::string> triggerList = MoodEntry::parseTriggers(rawTriggers);
            entries.emplace_back(id, stress, anxiety, note, triggerList, timestamp);
        }
    } else {
        std::cerr << "Fetch Error: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return entries;
}

// This function returns a list of "calculated" points
std::vector<MomentumPoint> DatabaseManager::getMoodMomentum() {
    std::vector<MomentumPoint> points;

    // We use created_at to order the 'window' chronologically
    std::string sql =
        "SELECT created_at, "
        "AVG(stress_level) OVER (ORDER BY created_at ROWS BETWEEN 6 PRECEDING AND CURRENT ROW), "
        "AVG(anxiety_level) OVER (ORDER BY created_at ROWS BETWEEN 6 PRECEDING AND CURRENT ROW) "
        "FROM mood_entry "
        "ORDER BY created_at DESC "
        "LIMIT 14;"; // We only show the last 2 weeks to keep the graph clean

    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            MomentumPoint p;
            
            // Extract the timestamp
            const unsigned char* dateText = sqlite3_column_text(stmt, 0);
            p.date = dateText ? reinterpret_cast<const char*>(dateText) : "Unknown";

            // Extract the calculated averages (column 1 and 2)
            p.rollingStress = static_cast<float>(sqlite3_column_double(stmt, 1));
            p.rollingAnxiety = static_cast<float>(sqlite3_column_double(stmt, 2));

            points.push_back(p);
        }
    } else {
        std::cerr << "Momentum Query Error: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return points;
}
