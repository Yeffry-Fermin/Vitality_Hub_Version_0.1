#pragma once
#include <sqlite3.h>
#include <vector>
#include <string>
#include "MoodEntry.h"

// Struct for the engine's time-series calculations
struct MomentumPoint {
    std::string date;
    float rollingStress;
    float rollingEnergy; // Added Energy
    float rollingSleep;  // Added Sleep (in hours)
};

class DatabaseManager {
private:
    sqlite3 *db; // Our connection to the database file

public:
    void createEntryTable();
    void createTriggerListTable();
    void createEntry(const MoodEntry& entry);
    std::vector<MoodEntry> getEntries(int days);
    std::vector<MomentumPoint> getMoodMomentum();
    
    DatabaseManager();  // Opens DB and calls the blueprints
    ~DatabaseManager(); // Closes the DB connection safely
};
