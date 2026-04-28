#pragma once
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
    void onViewAverages(int option);
    int getValidInt(std::string prompt, int min, int max);
    void onMostFrequentTrigger(int option);
    void onViewMoodTrends();
    std::string getProgressBar(float value, int maxScale = 10) const;
};

