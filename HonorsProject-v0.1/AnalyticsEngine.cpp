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

//Takes stress, energy and sleep, calculates a gap and uses all 3 variables to come up with messages
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
    // If we don't have at least two points, we can't calculate a change.
    // This prevents the app from crashing if the user has a brand new account.
    if (points.size() < 2) {
        return {0.0f, 0.0f, 0.0f};
    }

    // Since the SQL calculates rolling averages for every row:
    // points.front() = The most recent 7-day rolling average
    // points.back()  = The oldest 7-day rolling average in the window
    const auto& current = points.front();
    const auto& past    = points.back();

    float stressChange = 0.0f;
    float energyChange = 0.0f;
    float sleepChange  = 0.0f;

    // Formula for percent change ((Current - Past) / Past) * 100

    // 1. Stress Momentum
    if (past.rollingStress > 0) {
        stressChange = ((current.rollingStress - past.rollingStress) / past.rollingStress) * 100.0f;
    }

    // 2. Energy Momentum
    if (past.rollingEnergy > 0) {
        energyChange = ((current.rollingEnergy - past.rollingEnergy) / past.rollingEnergy) * 100.0f;
    }

    // 3. Sleep Momentum
    if (past.rollingSleep > 0) {
        sleepChange = ((current.rollingSleep - past.rollingSleep) / past.rollingSleep) * 100.0f;
    }

    return {stressChange, energyChange, sleepChange};
}

float AnalyticsEngine::calculateGlobalBaseline(const std::vector<MoodEntry>& entries) {
    if (entries.empty()) return 0.0f;
    
    return getAverageEnergy(entries);
}

// This function takes a list of daily mood entries and figures out which 'trigger' shows up most often then
std::vector<AnalyticsEngine::TriggerAnalysis> AnalyticsEngine::getTriggerInsights(const std::vector<MoodEntry> &entries) {
    std::vector<TriggerAnalysis> insights;
    if (entries.empty()) return insights;
    
    // 1. GLOBAL BASELINES (The Relational Context)
    // We calculate these once so we can compare every trigger against them.
    float globalEnergy = getAverageEnergy(entries);
    float globalSleep = getAverageSleep(entries);
    
    std::map<std::string, std::vector<MoodEntry>> triggerGroups;
    for (const auto& entry : entries) {
        for (const auto& trigger : entry.getTriggers()) {
            triggerGroups[trigger].push_back(entry);
        }
    }
    
    // Loop throuhg the map key: value pairs
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
        
        // Calculate the Delta
        float energyImpact = insight.avgEnergy - globalEnergy;
        
        // 3+ is required to notice a trend
        // Only analyze patterns that appear frequently enough
        // This prevents unreliable conclusions from tiny sample sizes
        if (insight.frequency >= 3) {

            // Measures how much sleep differs from the user's overall baseline
            // Stored as a percentage difference
            float sleepDiff = 0.0f;

            // Prevent division by zero before calculating percentage change
            if (globalSleep > 0) {

                // Formula:
                // ((current average - baseline average) / baseline average) * 100
                //
                // Positive result  -> more sleep than normal
                // Negative result  -> less sleep than normal
                sleepDiff = ((insight.avgSleep - globalSleep) / globalSleep) * 100.0f;
            }

            // PATTERN CLASSIFICATION SYSTEM
            // Case 1:
            // Strong negative energy impact combined with high stress
            // Indicates the activity is likely overwhelming or exhausting
            if (energyImpact <= -1.0f && insight.avgStress >= 3.5f) {

                insight.diagnosis = "CRITICAL STRAIN";

                insight.advice =
                    "This activity consistently drains your energy during high-stress periods. "
                    "Try pairing it with recovery time afterward.";
            }

            // Case 2:
            // Positive energy impact
            // Suggests the activity benefits energy and stability
            else if (energyImpact >= 1.0f) {

                insight.diagnosis = "HIGH EFFICIENCY";

                insight.advice =
                    "This activity appears to improve your energy and overall balance. "
                    "Consider making it part of your regular routine.";
            }

            // Case 3:
            // Moderate negative energy impact
            // Activity may contribute to fatigue over time
            else if (energyImpact <= -0.8f) {

                insight.diagnosis = "PARASITIC DRAIN";

                // If sleep is also significantly below baseline,
                // sleep deprivation may be worsening the problem
                if (sleepDiff <= -10.0f) {

                    insight.advice =
                        "Poor sleep may be amplifying the impact of this activity. "
                        "Prioritize recovery when possible.";
                }
                else {

                    insight.advice =
                        "This activity may be contributing to fatigue over time. "
                        "Monitor how often it appears during stressful periods.";
                }
            }

            // Case 4:
            // No major positive or negative impact detected
            else {

                insight.diagnosis = "STABLE PATTERN";

                insight.advice =
                    "This activity appears relatively balanced and manageable "
                    "within your current routine.";
            }
        }
        else {

            // Not enough observations to produce reliable analytics
            insight.diagnosis = "INSUFFICIENT DATA";

            insight.advice =
                "Log this activity a few more times to generate more accurate insights.";
        }
        
        insights.push_back(insight);
    }
    
    return insights;
}

AnalyticsEngine::CorrelationMetrics AnalyticsEngine::correlationLink(
    const std::vector<MoodEntry> &entries) {

    // Arrays used by ALGLIB for statistical calculations
    // Each array stores one variable across all mood entries
    alglib::real_1d_array energyPile;
    alglib::real_1d_array sleepPile;
    alglib::real_1d_array stressPile;

    // EXTRACT DATA FROM ENTRIES
    // Convert MoodEntry objects into raw numerical arrays
    // so ALGLIB can process them statistically

    extractCorrelationData(entries, energyPile, Mode::ENERGY);
    extractCorrelationData(entries, sleepPile, Mode::SLEEP);
    extractCorrelationData(entries, stressPile, Mode::STRESS);

    // PEARSON CORRELATION ANALYSIS
    // Pearson correlation measures how strongly two variables
    // move together on a scale from -1 to +1
    //
    // +1  -> strong positive relationship
    //  0  -> no relationship
    // -1  -> strong negative relationship

    double energySleepCorr = alglib::pearsoncorrelation(energyPile, sleepPile, entries.size());

    double energyStressCorr = alglib::pearsoncorrelation(energyPile, stressPile, entries.size());

    double sleepStressCorr = alglib::pearsoncorrelation(sleepPile, stressPile, entries.size());

    // MULTIPLE LINEAR REGRESSION
    // Goal:
    // Predict ENERGY using:
    //   - Stress
    //   - Sleep
    // Regression equation:
    // Energy =
    //   (stressSensitivity * Stress)
    // + (sleepEfficiency * Sleep)
    // + baseline

    // Regression matrix:
    // Column 0 -> Stress values
    // Column 1 -> Sleep values
    // Column 2 -> Constant intercept term (1.0)
    // Rows represent individual mood entries

    alglib::real_2d_array matrix;
    matrix.setlength((alglib::ae_int_t)entries.size(), 3);

    for (int i = 0; i < (int)entries.size(); i++) {

        // Predictor Variable #1
        matrix[i][0] = entries[i].getStressLevel();
        // Predictor Variable #2
        matrix[i][1] = entries[i].getSleepHours();
        // Constant intercept column
        matrix[i][2] = 1.0;
    }

    // Stores resulting regression coefficients
    alglib::real_1d_array c;

    // Diagnostic report returned by ALGLIB
    alglib::lsfitreport rep;

    // ------------------------------------------------------------
    // RUN LINEAR REGRESSION
    // ------------------------------------------------------------
    // ALGLIB solves for the coefficients that best fit:
    //
    // Energy =
    //   a(Stress)
    // + b(Sleep)
    // + c
    //
    // where:
    //   a = stressSensitivity
    //   b = sleepEfficiency
    //   c = baseline/intercept

    alglib::lsfitlinear(
        energyPile,
        matrix,
        (alglib::ae_int_t)entries.size(),
        3,
        c,
        rep);

    // Default fallback values
    double stressSensitivity = 0;
    double sleepEfficiency = 0;
    double baseline = 0;

    // Positive termination type means regression succeeded
    if (rep.terminationtype > 0) {

        // Regression coefficient for stress
        // Negative value:
        //   stress tends to reduce energy
        stressSensitivity = c[0];

        // Regression coefficient for sleep
        // Positive value:
        //   sleep tends to improve energy
        sleepEfficiency = c[1];

        // Baseline energy level when predictors are neutral
        baseline = c[2];
    }

    // Return all calculated analytics
    return {
        energySleepCorr,
        energyStressCorr,
        sleepStressCorr,
        stressSensitivity,
        sleepEfficiency,
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
                break;
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
