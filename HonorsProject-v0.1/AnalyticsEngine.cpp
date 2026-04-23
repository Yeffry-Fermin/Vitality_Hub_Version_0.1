#include "AnalyticsEngine.h"
#include "MoodEntry.h"

float AnalyticsEngine::getAverageStress(const std::vector<MoodEntry>& entries) const {
    if (entries.empty()) {
            return 0.0f;
    }
    
    
    float totalStress = 0.0f;
    for (const MoodEntry &entry : entries) {
        totalStress = totalStress + entry.getStressLevel();
    }
    
    return totalStress / entries.size();
}

float AnalyticsEngine::getAverageAnxiety(const std::vector<MoodEntry>& entries) const {
    if (entries.empty()) {
            return 0.0f;
    }
    
    
    float totalAnxiety = 0.0f;
    for (const MoodEntry &entry : entries) {
        totalAnxiety = totalAnxiety + entry.getAnxietyLevel();
    }
    
    return totalAnxiety / entries.size();
}
