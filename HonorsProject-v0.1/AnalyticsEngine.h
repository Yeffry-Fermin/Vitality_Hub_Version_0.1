#pragma once

#include <vector>
#include <string>
#include "MoodEntry.h"
#include "DatabaseManager.h"
#include "alglib/statistics.h" // library method to calc

enum class Mode {
    ENERGY = 0,
    SLEEP = 1,
    STRESS = 2
};

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
    struct CorrelationMetrics {
        double energySleep;
        double energyStress;
        double sleepStress;
        double sensitivity;
        double baseline;
    };
    float getAverageStress(const std::vector<MoodEntry>& entries) const;
    float getAverageEnergy(const std::vector<MoodEntry>& entries) const;
    float getAverageSleep(const std::vector<MoodEntry>& entries) const;
    std::string getVitalityAdvice(double stress, double energy, double sleep) const;
    std::vector<TriggerAnalysis> getTriggerInsights(const std::vector<MoodEntry>& entries);
    MoodAnalysis periodComparison(const std::vector<MomentumPoint>& points);
    CorrelationMetrics correlationLink(const std::vector<MoodEntry> &entries);
    void extractCorrelationData(const std::vector<MoodEntry>& entries, alglib::real_1d_array &outPile, Mode mode);
    std::string getInsight(double r, std::string varA, std::string varB);
};
