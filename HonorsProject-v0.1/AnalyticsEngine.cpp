#include "AnalyticsEngine.h"
#include "MoodEntry.h"
#include "alglib/statistics.h"
#include "alglib/interpolation.h"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
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
    // Difference between stress and energy
    // Positive = stress is higher than energy
    // Negative = energy is higher than stress
    double gap = stress - energy;
    
    std::string message = "";
    
    // Stress is significantly higher than energy (strong imbalance / overload)
    if (gap >= 2.0) {
        
        // Low sleep makes recovery worse
        if (sleep < 7.0) {
            message = "SYSTEM OVERLOAD: Your stress is high and your battery is empty. "
            "With low sleep, your foundation is weak. Rest is mandatory.";
        }
        // Sleep is fine, but stress is still too high relative to energy
        else {
            message = "HIGH STRAIN: You're well-rested, but your workload is "
            "outpacing your energy. Consider reducing workload.";
        }
    }
    // Stress is moderately higher than energy
    else if (gap >= 0.5) {
        
        // Low sleep increases risk of burnout
        if (sleep < 7.0) {
            message = "WARNING: Low sleep combined with rising stress. "
            "You should prioritize recovery.";
        }
        // Sleep is stable, system is under manageable pressure
        else {
            message = "STEADY LOAD: You're handling a moderate workload. "
            "Monitor energy levels to avoid fatigue.";
        }
    }
    // Energy and stress are roughly balanced
    else if (gap >= -1.0) {
        message = "STABLE: Energy and stress are balanced. "
        "This is a sustainable performance level.";
    }
    // Energy is higher than stress (good recovery / high capacity state)
    else {
        message = "HIGH ENERGY STATE: Energy is higher than stress. "
        "Good time for demanding tasks or productivity.";
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
std::vector<AnalyticsEngine::TriggerAnalysis> AnalyticsEngine::getTriggerInsights(const std::vector<MoodEntry> &entries) {
    std::vector<TriggerAnalysis> insights;
    if (entries.empty()) return insights;
    
    // 1. GLOBAL BASELINES (The Relational Context)
    // We calculate these once so we can compare every trigger against them.
    float globalEnergy = calculateGlobalBaseline(entries);
    float globalSleep = getAverageSleep(entries);
    
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
        
        // Calculate the Impact or Delta
        float energyImpact = insight.avgEnergy - globalEnergy;
        
        if (insight.frequency >= 3) { // 3+ is required to notice a trend

            // Calculate percentage difference: ((New - Old) / Old) * 100
            float sleepDiff = 0.0f;
            if (globalSleep > 0) {
                sleepDiff = ((insight.avgSleep - globalSleep) / globalSleep) * 100.0f;
            }
            
            if (energyImpact <= -1.0f && insight.avgStress >= 3.5f) {
                insight.diagnosis = "CRITICAL STRAIN";
                insight.advice = "Action: High load detected. Schedule a 'Power Source' activity (like Gym) after this.";
            }
            else if (energyImpact >= 1.0f) {
                insight.diagnosis = "HIGH EFFICIENCY";
                insight.advice = "Action: This is a recharge point. Protect this time in your schedule.";
            }
            else if (energyImpact <= -0.8f) {
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

AnalyticsEngine::CorrelationMetrics AnalyticsEngine::correlationLink(const std::vector<MoodEntry> &entries) {
    alglib::real_1d_array energyPile;
    alglib::real_1d_array sleepPile;
    alglib::real_1d_array stressPile;
    
    // Piles are passed by ref so we avoid making copies, now the functions above are gud
    extractCorrelationData(entries, energyPile, Mode::ENERGY);
    extractCorrelationData(entries, sleepPile, Mode::SLEEP);
    extractCorrelationData(entries, stressPile, Mode::STRESS);
    
    // "pearson correlation formula" - formula measures the strength and direction of a linear relationship between two variables,
    // ranging from -1 to +1.
    double energySleepCorr = alglib::pearsoncorrelation(energyPile, sleepPile, entries.size());
    double energyStressCorr = alglib::pearsoncorrelation(energyPile, stressPile, entries.size());
    double sleepStressCorr = alglib::pearsoncorrelation(sleepPile, stressPile, entries.size());
    
    
    alglib::real_2d_array matrix;
    matrix.setlength((alglib::ae_int_t)entries.size(), 2);
    
    for (int i = 0; i < entries.size(); i++) {
        matrix[i][0] = entries[i].getStressLevel();
        matrix[i][1] = 1.0;
    }
    
    alglib::real_1d_array c;
    alglib::lsfitreport rep;
    alglib::lsfitlinear(energyPile, matrix, (alglib::ae_int_t)entries.size(), 2, c, rep);
    
    double sensitivity = 0;
    double baseline = 0;
    
    if (rep.terminationtype > 0) {
        sensitivity = c[0]; // (Slope)
        baseline = c[1];    // (Intercept)
    }
    
    return {
        energySleepCorr,
        energyStressCorr,
        sleepStressCorr,
        sensitivity,
        baseline
    };
}

// Helper function to extra pile data from mood entry vector
void AnalyticsEngine::extractCorrelationData(const std::vector<MoodEntry> &entries, alglib::real_1d_array &outPile, Mode mode) {
    int size = entries.size();
    outPile.setlength(size);
    
    for (int i = 0; i < size; i++) {
        switch (mode) {
            case Mode::ENERGY:
                outPile[i] = entries[i].getEnergyLevel();
                break;
            case Mode::SLEEP:
                outPile[i] = entries[i].getSleepHours();
                break;
            case Mode::STRESS:
                outPile[i] = entries[i].getStressLevel();
            default:
                break;
        }
    }
}

// Takes correlation rating + 2 links and outputs message
std::string AnalyticsEngine::getInsight(double cRating, std::string varA, std::string varB) {
    std::stringstream stream;
    
    stream << std::fixed << std::setprecision(2) << cRating;
    
    std::string r_clean = stream.str();

    if (cRating >= 0.7) {
        return "System Report: The data identifies a Strong Positive correlation (r = " + r_clean + ") "
        "between " + varA + " and " + varB + ". In practice, higher " + varA +
        " levels are consistently associated with higher " + varB + " levels. "
        "This suggests these metrics tend to improve together over time.";
    }
    else if (cRating >= 0.3) {
        return "System Report: The data identifies a Moderate Positive correlation (r = " + r_clean + ") "
        "between " + varA + " and " + varB + ". The data shows a noticeable trend where increases in "
        + varA + " are often linked with increases in " + varB + ". "
        "While the relationship is not perfect, the connection is still meaningful.";
    }
    else if (cRating > -0.3) {
        return "System Report: The data suggests a Weak or Negligible relationship (r = " + r_clean + ") "
        "between " + varA + " and " + varB + ". Changes in " + varA +
        " do not consistently predict changes in " + varB + " within the current dataset.";
    }
    else if (cRating <= -0.7) {
        return "System Report: The data identifies a Strong Negative correlation (r = " + r_clean + ") "
        "between " + varA + " and " + varB + ". In practice, higher " + varA +
        " levels are consistently associated with lower " + varB + " levels. "
        "This suggests that improvements in one area may help stabilize the other.";
    }
    else {
        return "System Report: The data identifies a Moderate Negative correlation (r = " + r_clean + ") "
        "between " + varA + " and " + varB + ". The data suggests that increases in "
        + varA + " are often linked with decreases in " + varB + ". "
        "The relationship appears noticeable, though not strongly consistent.";
    }
}
