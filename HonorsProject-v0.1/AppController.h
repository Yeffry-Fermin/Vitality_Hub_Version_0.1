#pragma once
#include "DatabaseManager.h"
#include "AnalyticsEngine.h"
#include <string> // Added this just to be safe for your prompts!

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
    double getValidDouble(std::string prompt, double min, double max);
    
    void onMostFrequentTrigger(int option);
    void onViewMoodTrends();
    std::string getProgressBar(float value, int maxScale = 10) const;
};
