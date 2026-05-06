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
        message = "PEAK CAPACITY: You have a massive energy surplus. This is the perfect "
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
        "between " + varA + " and " + varB + ". Statistically, this indicates that " + varA +
        " acts as a primary driver for " + varB + " levels within the observed period. "
        "From an optimization standpoint, " + varA + " should be prioritized as a key metric "
        "for improving overall system stability.";
    }
    else if (cRating >= 0.3) {
        return "System Report: The data identifies a Moderate Positive correlation (r = " + r_clean + ") "
        "between " + varA + " and " + varB + ". This suggests a visible trend where " + varA +
        " influences " + varB + " levels, though secondary environmental or behavioral "
        "factors likely contribute to the remaining variance.";
    }
    else if (cRating > -0.3) {
        return "System Report: The data suggests a Weak or Negligible relationship (r = " + r_clean + ") "
        "between " + varA + " and " + varB + ". Statistically, these metrics appear to operate "
        "independently within the current dataset; changes in " + varA + " do not reliably "
        "predict shifts in " + varB + " at this time.";
    }
    else if (cRating <= -0.7) {
        return "System Report: The data identifies a Strong Negative correlation (r = " + r_clean + ") "
        "between " + varA + " and " + varB + ". Statistically, this indicates an inverse relationship "
        "where " + varA + " acts as a primary systemic drain on " + varB + " levels. "
        "From an optimization standpoint, " + varA + " should be identified as a critical factor "
        "for mitigation to prevent the further destabilization of " + varB + ".";
    }
    else {
        return "System Report: The data identifies a Moderate Negative correlation (r = " + r_clean + ") "
        "between " + varA + " and " + varB + ". This indicates an inverse relationship "
        "where " + varA + " exerts moderate downward pressure on " + varB + " levels, "
        "suggesting that increases in " + varA + " may partially inhibit " + varB + " stability.";
    }
}
