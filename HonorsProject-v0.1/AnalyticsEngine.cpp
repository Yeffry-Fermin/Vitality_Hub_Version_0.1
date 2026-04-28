#include "AnalyticsEngine.h"
#include "MoodEntry.h"
#include <string>
#include <vector>
#include <sstream>
#include <map>

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

AnalyticsEngine::TriggerInsight AnalyticsEngine::getTriggerInsights(const std::vector<MoodEntry>& entries) {
    // 1. Create a map to store the data for EVERY trigger we find
    std::map<std::string, TriggerInsight> insightMap;

    // 2. The Outer Loop: Go through every log entry
    for (const auto& entry : entries) {
        // 3. The Inner Loop: Go through every trigger word in that entry
        for (const auto& triggerName : entry.getTriggers()) {
            
            // Access the stats for this specific trigger (creates it if it doesn't exist)
            TriggerInsight& stats = insightMap[triggerName];

            // 4. Update the data
            stats.trigger = triggerName;
            stats.frequency++;
            // We'll store the SUM here for now, and divide later to get the average
            stats.avgStress += entry.getStressLevel();
            stats.avgAnxiety += entry.getAnxietyLevel();
        }
    }

    // 5. Find the "Winner" (the one with the highest frequency)
    TriggerInsight topTrigger = {"None", 0, 0.0f, 0.0f};

    for (auto& pair : insightMap) {
        TriggerInsight& current = pair.second;
        
        // Finalize the averages by dividing the totals by the frequency
        current.avgStress /= current.frequency;
        current.avgAnxiety /= current.frequency;

        // Check if this trigger is more frequent than our current "top" one
        if (current.frequency > topTrigger.frequency) {
            topTrigger = current;
        }
    }

    return topTrigger;
}
