#pragma once
#include <sqlite3.h>
#include "moodEntry.h"

class DatabaseManager {
private:
    sqlite3 *db; // Our connection to the database file

public:
    void createEntryTable();
    void createTriggerListTable();
    void createBridgeTable();

    // --- Data Management Functions (The Actions) ---
    void createEntry(const MoodEntry& entry); // Packages and saves everything

    DatabaseManager();  // Opens DB and calls the blueprints
    ~DatabaseManager(); // Closes the DB connection safely
};
