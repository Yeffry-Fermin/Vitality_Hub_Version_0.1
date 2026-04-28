#pragma once

#include <vector>
#include <string>
#include "MoodEntry.h"

class AnalyticsEngine {
private:
public:
    struct TriggerInsight{
        std::string trigger;
        int frequency;
        float avgStress;
        float avgAnxiety;
    };
    TriggerInsight getTriggerInsights(const std::vector<MoodEntry>& entries);
    float getAverageStress(const std::vector<MoodEntry>& entries) const;
    float getAverageAnxiety(const std::vector<MoodEntry>& entries) const;
    
};

