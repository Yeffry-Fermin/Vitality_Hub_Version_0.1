#pragma
#include "DatabaseManager.h"
#include "trendAnalyzer.h"
#include <string>

class AppController {
private:
    DatabaseManager& db;
public:
    AppController(DatabaseManager& dbInstance);
    void onAddMoodEntry();   // Logic to gather input and save to file
    void onViewHistory(); // Logic to trigger the analyzer and show results
    int getValidInt(string prompt, int min, int max);
};

