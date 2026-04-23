#pragma
#include "DatabaseManager.h"
#include "AnalyticsEngine.h"

class AppController {
private:
    DatabaseManager& db;
    AnalyticsEngine& analytics;
public:
    AppController(DatabaseManager& dbInstance, AnalyticsEngine& analytics);
    void onAddMoodEntry();
    void onViewHistory();
    void onViewAnalytics();
    void onViewAverages();
    int getValidInt(std::string prompt, int min, int max);
};

