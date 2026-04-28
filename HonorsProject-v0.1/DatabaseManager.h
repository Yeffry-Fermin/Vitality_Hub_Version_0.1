#pragma once
#include <sqlite3.h>
#include "MoodEntry.h"

//temp place for this struct
struct MomentumPoint {
    std::string date;
    float rollingStress;
    float rollingAnxiety;
};

struct Momentum {
    
}

class DatabaseManager {
private:
    sqlite3 *db; // Our connection to the database file

public:
    void createEntryTable();
    void createTriggerListTable();
    void createEntry(const MoodEntry& entry);
    std::vector<MoodEntry>getEntries(int days);
    std::vector<MomentumPoint> getMoodMomentum();
    DatabaseManager();  // Opens DB and calls the blueprints
    ~DatabaseManager(); // Closes the DB connection safely
};
