#pragma once

#include <vector>
#include <string>
#include "MoodEntry.h"
#include "DatabaseManager.h"

class AnalyticsEngine {
private:
    float calculateGlobalBaseline(const std::vector<MoodEntry>& entries);
public:
    struct TriggerAnalysis {
        std::string trigger;
        int frequency;
        float avgStress;
        float avgEnergy;
        float avgSleep;
        float globalEnergy;
        float globalSleep;
        std::string diagnosis;
        std::string advice;
    };
    
    struct MoodAnalysis {
        float stressChange;
        float energyChange;
        float sleepChange;
    };
    float getAverageStress(const std::vector<MoodEntry>& entries) const;
    float getAverageEnergy(const std::vector<MoodEntry>& entries) const;
    float getAverageSleep(const std::vector<MoodEntry>& entries) const;
    std::string getVitalityAdvice(double stress, double energy, double sleep) const;
    std::vector<TriggerAnalysis> getTriggerInsights(const std::vector<MoodEntry>& entries);
    MoodAnalysis periodComparison(const std::vector<MomentumPoint>& points);
};
