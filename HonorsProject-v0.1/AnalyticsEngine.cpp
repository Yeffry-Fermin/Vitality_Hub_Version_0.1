#include "AnalyticsEngine.h"
#include "MoodEntry.h"
#include <string>
#include <vector>
#include <sstream>
#include <map>


float AnalyticsEngine::getAverageStress(const std::vector<MoodEntry>& entries) const {
    if (entries.empty()) return 0.0f;
    
    float totalStress = 0.0f;
    for (const MoodEntry &entry : entries) {
        totalStress += entry.getStressLevel();
    }
    return totalStress / entries.size();
}

float AnalyticsEngine::getAverageEnergy(const std::vector<MoodEntry>& entries) const {
    if (entries.empty()) return 0.0f;
    
    float totalEnergy = 0.0f;
    for (const MoodEntry &entry : entries) {
        totalEnergy += entry.getEnergyLevel();
    }
    return totalEnergy / entries.size();
}

float AnalyticsEngine::getAverageSleep(const std::vector<MoodEntry>& entries) const {
    if (entries.empty()) return 0.0f;
    
    float totalSleep = 0.0f;
    for (const MoodEntry &entry : entries) {
        totalSleep += entry.getSleepHours();
    }
    return totalSleep / entries.size();
}

std::string AnalyticsEngine::getVitalityAdvice(double stress, double energy, double sleep) const {
    double gap = stress - energy;
    std::string message = "";

    // ZONE 1: CRITICAL OVERLOAD (Stress is much higher than Energy)
    if (gap >= 2.0) {
        if (sleep < 7.0) {
            message = "SYSTEM OVERLOAD: Your stress is high and your battery is empty. "
                      "With low sleep, your foundation is weak. Rest is mandatory.";
        } else {
            message = "HIGH STRAIN: You're well-rested, but your current workload is "
                      "outpacing your energy. Consider dropping a non-essential task.";
        }
    }
    // ZONE 2: MODERATE STRAIN (Stress is slightly higher than Energy)
    else if (gap >= 0.5) {
        if (sleep < 7.0) {
            message = "WARNING: You're pushing through on low sleep. The gap is widening; "
                      "try to catch up on rest tonight to avoid burnout.";
        } else {
            message = "STEADY EFFORT: You're managing a heavy load. You have the sleep "
                      "foundation to handle this, but keep an eye on your energy levels.";
        }
    }
    // ZONE 3: SUSTAINABLE BALANCE (Energy and Stress are roughly equal)
    else if (gap >= -1.0) {
        message = "STABLE: You're in a great rhythm. Your energy and stress are well-balanced. "
                  "This is a sustainable pace for long-term productivity.";
    }
    // ZONE 4: PEAK CAPACITY (Energy is significantly higher than Stress)
    else {
        message = "PEAK CAPACITY: You have a massive energy surplus! This is the perfect "
                  "time to tackle your hardest project or a difficult study session.";
    }

    return message;
}


AnalyticsEngine::MoodAnalysis AnalyticsEngine::periodComparison(const std::vector<MomentumPoint> &points) {
    // 1. Check if 14 days worth of data is passed
    if (points.size() < 14) return {0.0f, 0.0f, 0.0f};

    auto split_it = points.begin() + 7;
    std::vector<MomentumPoint> previous(split_it, points.end());
    std::vector<MomentumPoint> current(points.begin(), split_it);
    
    // 2. Initialization
    float totalPreviousStress = 0.0f, totalCurrentStress = 0.0f;
    float totalPreviousEnergy = 0.0f, totalCurrentEnergy = 0.0f;
    float totalPreviousSleep = 0.0f,  totalCurrentSleep = 0.0f;
    
    // Sum up the current week
    for (const auto &point : current) {
        totalCurrentStress += point.rollingStress;
        totalCurrentEnergy += point.rollingEnergy;
        totalCurrentSleep  += point.rollingSleep;
    }
    
    // Sum up the previous week
    for (const auto &point : previous) {
        totalPreviousStress += point.rollingStress;
        totalPreviousEnergy += point.rollingEnergy;
        totalPreviousSleep  += point.rollingSleep;
    }
    
    float stressChange = 0.0f, energyChange = 0.0f, sleepChange = 0.0f;
    
    if (totalPreviousStress > 0) {
        stressChange = ((totalCurrentStress - totalPreviousStress) / totalPreviousStress) * 100.0f;
    }
    if (totalPreviousEnergy > 0) {
        energyChange = ((totalCurrentEnergy - totalPreviousEnergy) / totalPreviousEnergy) * 100.0f;
    }
    if (totalPreviousSleep > 0) {
        sleepChange = ((totalCurrentSleep - totalPreviousSleep) / totalPreviousSleep) * 100.0f;
    }

    // 4. Return the full Triad Analysis
    return {stressChange, energyChange, sleepChange};
}

// helper function to help calculate the global baseline feature

float AnalyticsEngine::calculateGlobalBaseline(const std::vector<MoodEntry>& entries) {
    if (entries.empty()) return 0.0f;
    
    return getAverageEnergy(entries);
}

// This function takes a list of daily mood entries and figures out which 'trigger' shows up most often.
std::vector<AnalyticsEngine::TriggerAnalysis> AnalyticsEngine::getTriggerInsights(const std::vector<MoodEntry>& entries) {
    std::vector<TriggerAnalysis> insights;
    if (entries.empty()) return insights;

    // 1. GLOBAL BASELINES (The Relational Context)
    // We calculate these once so we can compare every trigger against them.
    float globalEnergy = calculateGlobalBaseline(entries);
    float globalSleep = getAverageSleep(entries);

    // 2. GROUPING (The Filing Cabinet)
    std::map<std::string, std::vector<MoodEntry>> triggerGroups;
    for (const auto& entry : entries) {
        for (const auto& trigger : entry.getTriggers()) {
            triggerGroups[trigger].push_back(entry);
        }
    }

    // 3. ANALYSIS LOOP
    for (auto const& [name, subset] : triggerGroups) {
        TriggerAnalysis insight;
        insight.trigger = name;
        insight.frequency = subset.size();
        
        // Local averages for this specific activity
        insight.avgStress = getAverageStress(subset);
        insight.avgEnergy = getAverageEnergy(subset);
        insight.avgSleep = getAverageSleep(subset);

        // Store global values in the struct for the UI to display
        insight.globalEnergy = globalEnergy;
        insight.globalSleep = globalSleep;

        // MATH: Calculate the Impact (Delta)
        float energyImpact = insight.avgEnergy - globalEnergy;

        // 4. DIAGNOSIS & ADVICE (The Three Elements)
        if (insight.frequency >= 3) {
            // ELEMENT 1: Sleep Analysis (The "Why" Insight)
            // Calculate percentage difference: ((New - Old) / Old) * 100
            float sleepDiff = 0.0f;
            if (globalSleep > 0) {
                sleepDiff = ((insight.avgSleep - globalSleep) / globalSleep) * 100.0f;
            }

            // ELEMENT 2 & 3: Diagnosis + Call to Action
            if (energyImpact <= -1.0f && insight.avgStress >= 3.5f) {
                insight.diagnosis = "CRITICAL STRAIN";
                insight.advice = "Action: High load detected. Schedule a 'Power Source' activity (like Gym) after this.";
            }
            else if (energyImpact >= 1.0f) {
                insight.diagnosis = "HIGH EFFICIENCY";
                insight.advice = "Action: This is a recharge point. Protect this time in your schedule.";
            }
            else if (energyImpact <= -0.8f) { // Slightly tighter threshold than 1.0
                insight.diagnosis = "RESOURCE DRAIN";
                
                // Tailor advice based on the sleep analysis
                if (sleepDiff <= -10.0f) {
                    insight.advice = "Action: Sleep deficit detected. Prioritize recovery over extra work hours.";
                } else {
                    insight.advice = "Action: Battery leak detected. Review if this activity can be optimized.";
                }
            }
            else {
                insight.diagnosis = "STABLE FLOW";
                insight.advice = "Action: Sustainable rhythm. Keep maintaining current recovery habits.";
            }
        } else {
            insight.diagnosis = "INSUFFICIENT DATA";
            insight.advice = "Action: Log at least 3 occurrences to unlock diagnostic insights.";
        }

        insights.push_back(insight);
    }

    return insights;
}
