#pragma
#include "DatabaseManager.h"
#include "trendAnalyzer.h"
#include <string>

class AppController {
private:
    DatabaseManager& db;
public:
    AppController(DatabaseManager& dbInstance);
    void onAddMoodEntry();
    void onViewHistory();
    void onViewAnalytics();
    int getValidInt(string prompt, int min, int max);
};

