#include <string>
#include <iostream>
#include <limits>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cmath>

#include "MoodEntry.h"
#include "AppController.h"
#include "DatabaseManager.h"
#include "AnalyticsEngine.h"
#include "AverageReport.h"

// Updated constructor to handle both dependencies
AppController::AppController(DatabaseManager& dbInstance, AnalyticsEngine& analyticsInstance)
: db(dbInstance), analytics(analyticsInstance)
{
}

void AppController::onAddMoodEntry() {
    
    std::cout << "----------------\n";
    std::cout << "VITALITY SYSTEM CHECK-IN\n";
    
    std::string triggers;
    // Clear the buffer once at the start to ensure getline works
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    do {
        std::cout << "What just happened? Enter the main activity/event (e.g., Exam, Gym, Work): ";
        getline(std::cin, triggers);
    } while (triggers.length() < 2);
    
    std::vector<std::string> parsedTriggers = MoodEntry::parseTriggers(triggers);
    
    int energyLevel = getValidInt("Current 'Energy' level? (1 = Exhausted, 5 = Full): ", 1, 5);
    
    int stressLevel = getValidInt("Current 'Load' or stress? (1 = Light, 5 = Heavy): ", 1, 5);
    
    double sleepHours = getValidDouble("Recovery Check: How many hours of sleep did you get last night?: ", 0.0, 24.0);
    
    std::cout << "Any quick notes about this specific event? (Press Enter to skip): ";
    std::string note;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    getline(std::cin, note);
    
    std::cout <<"---------------------------\n";
    
    // Save the entry with the updated 1-5 scales
    MoodEntry newEntry(stressLevel, energyLevel, sleepHours, note, parsedTriggers);
    
    try {
        db.createEntry(newEntry);
        std::cout << "Log successful. Vitality data synchronized.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cout << "Failed to save entry. Please try again.\n";
    }
    
}
// Showcase of your logs
void AppController::onViewHistory() {
    std::vector<MoodEntry> history = db.getEntries(0);
    
    if (history.empty()) {
        std::cout << "\nNo logs found yet. Start by adding your first entry\n";
        return;
    }
    
    std::cout << std::left << std::setw(4)  << "ID"
    << std::setw(22) << "Date"
    << std::setw(8)  << "Stress"
    << std::setw(8)  << "Energy"
    << std::setw(8)  << "Sleep"
    << "Note / Triggers" << std::endl;
    
    std::cout << std::string(80, '-') << std::endl;
    
    for(const auto& entry: history) {
        std::cout << std::left << std::setw(4)  << entry.getId()
        << std::setw(22) << entry.getTimestamp()
        << std::setw(8)  << entry.getStressLevel()
        << std::setw(8)  << entry.getEnergyLevel()
        << std::setw(8)  << std::fixed << std::setprecision(1) << entry.getSleepHours();
        
        std::vector<std::string> triggers = entry.getTriggers();
        int lastIndex = (int)triggers.size() - 1;
        
        for (int i = 0; i < triggers.size(); i++) {
            std::cout << triggers[i];
            if (i < lastIndex) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }
}


void AppController::onViewAverages(int option) {
    
    std::vector<MoodEntry> entries = db.getEntries(option);
    
    if(entries.empty()) {
        std::cout << "Not enough data in the system\n";
        return;
    }
    
    float avgStress = analytics.getAverageStress(entries);
    float avgEnergy = analytics.getAverageEnergy(entries);
    float avgSleep = analytics.getAverageSleep(entries);
    
    std::string statusMessage = analytics.getVitalityAdvice(avgStress,avgEnergy,avgSleep);
    
    AverageReport report(avgStress,avgEnergy,avgSleep,statusMessage);
    
    report.generate();
}


void AppController::onMostFrequentTrigger(int option) {
    std::vector<MoodEntry> entries = db.getEntries(option);
    std::vector<AnalyticsEngine::TriggerAnalysis> insights = analytics.getTriggerInsights(entries);

    std::cout << "\n========================================\n";
    std::cout << "             Pattern Finder                ";
    std::cout << "\n========================================\n";

    if (insights.empty()) {
        std::cout << " No data patterns identified yet. Keep logging!" << std::endl;
        std::cout << "========================================\n";
        return;
    }
    // Sort by frequency so the most common activity is first
    // 3rd param lambda function to tell it how to order the values a first b second
    std::sort(insights.begin(), insights.end(), [](const auto& a, const auto& b) {
        return a.frequency > b.frequency;
    });
    
    //here i can easily get the 2nd most common and 3rd most common
    const auto& top = insights[0];

    // Create readable time label
    std::string timeLabel;

    if (option == 0) {
        timeLabel = "All Time";
    } else {
        timeLabel = std::to_string(option) + " days";
    }

    std::cout << "Top Pattern Identified: \"" << top.trigger << "\"\n";
    std::cout << "----------------------------------------\n";

    std::cout << " Frequency:      "
              << top.frequency
              << " times ("
              << timeLabel
              << ")\n";

    std::cout << std::fixed << std::setprecision(1);

    std::cout << " Avg. Battery:   " << top.avgEnergy
              << " (Normal: " << top.globalEnergy << ")\n";

    std::cout << " Avg. Load:      " << top.avgStress << " / 5\n";
    std::cout << " Avg. Sleep:     " << top.avgSleep << " hours\n\n";

    std::cout << "ANALYSIS: ";

    if (top.globalSleep > 0) {
        float sleepDiff = ((top.avgSleep - top.globalSleep) / top.globalSleep) * 100.0f;

        if (sleepDiff <= -10.0f) {
            std::cout << "Your sleep is "
                      << std::abs(sleepDiff)
                      << "% below your baseline. "
                      << "This may be increasing fatigue during this activity.\n";
        } else {
            std::cout << "Sleep levels are stable. "
                      << "The stress impact appears more activity-related.\n";
        }
    }
    // TOO MUCH INFO LETS LEAVE IT AT THAT FOR NOW UI I ADD THIS
//    float energyDelta = top.avgEnergy - top.globalEnergy;
//
//    if (energyDelta <= -1.0f) {
//        std::cout << "CRITICAL DRAIN: This activity pulls your battery "
//                  << std::abs(energyDelta) << " points below your normal baseline.\n";
//    }
//    else if (energyDelta >= 1.0f) {
//        std::cout << "RECOVERY BOOST: This activity increases your battery by "
//                  << energyDelta << " points above your baseline.\n";
//    }
//    else {
//        std::cout << "NEUTRAL IMPACT: Your battery levels remain stable during this activity.\n";
//    }
    
    

    std::cout << "\nDIAGNOSIS: " << top.diagnosis << "\n";
    std::cout << "ACTION: " << top.advice << "\n";

    std::cout << "========================================\n\n";
}
void AppController::onViewMoodTrends() {
    std::vector<MomentumPoint> points = db.getMoodMomentum();
    
    std::cout << "\n========================================\n";
    std::cout << "               Mood Momentum                ";
    std::cout << "\n========================================\n";
    
    std::cout << std::left << std::setw(12) << "DATE" << " | [ STRESS ] | [ ENERGY ] | SLEEP\n";
    
    for (auto it = points.rbegin(); it != points.rend(); ++it) {
        std::cout << std::left << std::setw(12) << it->date.substr(0, 10) << " | ";
        
        // 2. Stress Bar
        int sWidth = (int)it->rollingStress;
        for(int i=0; i<10; i++) std::cout << (i < sWidth ? "█" : " ");
        std::cout << " | ";
        
        // 3. Energy Bar
        int eWidth = (int)(it->rollingEnergy * 2);
        for(int i=0; i<10; i++) std::cout << (i < eWidth ? "█" : " ");
        
        // 4. Sleep hours
        std::cout << " | " << std::fixed << std::setprecision(1) << it->rollingSleep << " hrs" << std::endl;
    }
    
    auto dataAnalysis = analytics.periodComparison(points);
    
    // --- STRESS TREND ---
    if (dataAnalysis.stressChange < 0) {
        std::cout << "\nSuccess: Your stress dropped " << std::fixed << std::setprecision(1) << -(dataAnalysis.stressChange) << "% compared to last week. \n";
    } else if (dataAnalysis.stressChange > 0) {
        std::cout << "\nNotice: Your stress increased by " << std::fixed << std::setprecision(1) << dataAnalysis.stressChange << "% recently. \n";
    } else {
        std::cout << "\nStability: Your stress levels have remained consistent with last week. \n";
    }

    // --- ENERGY TREND ---
    if (dataAnalysis.energyChange > 0) {
        std::cout << "Success: Your energy is up " << std::fixed << std::setprecision(1) << dataAnalysis.energyChange << "%! \n";
    } else if (dataAnalysis.energyChange < 0) {
        std::cout << "Warning: Your energy has dropped by " << std::fixed << std::setprecision(1) << -(dataAnalysis.energyChange) << "%. \n";
    } else {
        std::cout << "Stability: Your energy levels are maintaining a steady equilibrium. \n";
    }

    // --- SLEEP TREND ---
    if (dataAnalysis.sleepChange > 0) {
        std::cout << "Insight: You are sleeping " << std::fixed << std::setprecision(1) << dataAnalysis.sleepChange << "% more on average. \n";
    } else if (dataAnalysis.sleepChange < 0) {
        std::cout << "Insight: Your sleep is down " << std::fixed << std::setprecision(1) << -(dataAnalysis.sleepChange) << "% compared to last week. \n";
    } else {
        std::cout << "Insight: Your sleep schedule has remained perfectly stable. \n";
    }
}

// Helper function for integer prompts
int AppController::getValidInt(std::string prompt, int min, int max) {
    int value;
    bool isValid;
    do {
        isValid = true;
        std::cout << prompt;
        if (!(std::cin >> value) || value < min || value > max) {
            isValid = false;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "Error: Please enter a number between " << min << " and " << max << "." << std::endl;
        }
    } while (!isValid);
    return value;
}

// Helper function for double prompts
double AppController::getValidDouble(std::string prompt, double min, double max) {
    double value;
    bool isValid;
    do {
        isValid = true;
        std::cout << prompt;
        if (!(std::cin >> value) || value < min || value > max) {
            isValid = false;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cerr << "Error: Please enter a valid number between " << min << " and " << max << "." << std::endl;
        }
    } while (!isValid);
    return value;
}
//function to generate simple bar graph of #'s
std::string AppController::getProgressBar(float value, int maxScale) const {
    int filledLength = static_cast<int>(value);
    if (filledLength > maxScale) filledLength = maxScale; // Safety constraint
    int emptyLength = maxScale - filledLength;
    
    std::string filled(filledLength, '#');
    std::string empty(emptyLength, '-');
    
    return "[" + filled + empty + "]";
}

void AppController::onViewCorrelationReport() {
    const int colL = 25;
    const int colI = 15;

    // Load data
    std::vector<MoodEntry> entries = db.getEntries(0);
    auto metrics = analytics.correlationLink(entries);

    // UI Header
    std::cout << "\n================================================\n";
    std::cout << "             Behavior Pattern Report             \n";
    std::cout << "================================================\n";
    
    // 1. DATA TABLE
    std::cout << std::left << std::setw(colL) << "[ Relationship ]"
              << std::setw(colI) << "[ Correlation ]"
              << "[ TYPE ]\n";
    std::cout << "------------------------------------------------\n";

    auto printRow = [&](std::string label, double val) {
        std::cout << std::left << std::setw(colL) << label
                  << "|    " << std::setw(8) << std::fixed << std::setprecision(2) << val
                  << "|  " << getTypeLabel(val) << "\n";
    };

    printRow("Sleep -> Energy", metrics.energySleep);
    printRow("Stress -> Energy", metrics.energyStress);
    printRow("Sleep -> Stress", metrics.sleepStress);
    std::cout << "================================================\n";

    // 2. PRIMARY DRIVER (Consolidated Result & Detailed Analysis)
    double strongestR = metrics.energySleep;
    std::string varA = "Energy", varB = "Sleep";

    if (std::abs(metrics.energyStress) > std::abs(strongestR)) {
        strongestR = metrics.energyStress; varA = "Energy"; varB = "Stress";
    }
    if (std::abs(metrics.sleepStress) > std::abs(strongestR)) {
        strongestR = metrics.sleepStress; varA = "Sleep"; varB = "Stress";
    }

    std::cout << "Primary System Driver\n";
    std::cout << "The strongest link is " << varA << " <-> " << varB << " (r = " << std::abs(strongestR) << ").\n";
    std::cout << "Impact: " << (strongestR > 0 ? "Positive Correlation. These metrics move together."
                                                : "Inverse Relationship. These metrics move in opposite directions.") << "\n";

    std::cout << "------------------------------------------------\n";
    std::cout << "MODEL INSIGHTS (Linear Regression):\n";


    std::cout << std::fixed << std::setprecision(1);

    double stressCoef = metrics.stressSensitivity;
    double sleepCoef = metrics.sleepEfficiency;

    // Stress output
    std::cout << " - Stress Impact: "
              << "Each +1 stress point is associated with a "
              << std::abs(stressCoef) / 4.0 * 100
              << "% ";

    if (stressCoef < 0)
        std::cout << "decrease in energy.\n";
    else
        std::cout << "increase in energy.\n";

    // Sleep output
    std::cout << " - Sleep Impact: "
              << "Each +1 hour of sleep is associated with a "
              << (sleepCoef / 4.0) * 100
              << "% ";

    if (sleepCoef < 0)
        std::cout << "decrease in energy.\n";
    else
        std::cout << "increase in energy.\n";

//    // Dynamic Status Logic
//    std::cout << "\nSTATUS: ";
//    if (std::abs(metrics.stressSensitivity) > metrics.sleepEfficiency) {
//        std::cout << "High Fragility. Stress drain outpaces sleep recovery.\n";
//    } else {
//        std::cout << "High Resilience. Sleep recovery efficiently offsets stress.\n";
//    }
//    std::cout << "------------------------------------------------\n";
    
    printCorrelationLegend();
}
std::string AppController::toPct(double val) {
    // Multiply by 100 and cast to int to remove decimals
    int percentage = (int)(val * 100);
    return std::to_string(percentage) + "%";
}

std::string AppController::getTypeLabel(double val) {
    double strength = std::abs(val);
    
    // 1. If it's too small, it's effectively "No Correlation"
    if (strength < 0.3) {
        return "None";
    }
    
    // 2. If it's positive and strong enough
    if (val > 0) {
        return "Positive"; // Shortened to fit the table width
    }
    
    // 3. If it's negative and strong enough
    return "Negative";
}

void AppController::printSensitivityBlock(double sensitivity) {
    double absSens = std::abs(sensitivity); // sensitivity is essentially the slope
    
    std::string level;
    if (absSens >= 0.7) {
        level = "High";
    } else if (absSens >= 0.4){
        level = "Moderate";
    } else {
        level = "Low";
    }
    
    // 2. Determine the Directional Verb
    std::string verb = (sensitivity < 0) ? "drops" : "increases";
    
    // 3. The Modular Output
    std::cout << "Exchange Cost: \n";
    std::cout << level << " sensitivity to Load spikes. For every \n";
    std::cout << "1.0 point of Stress added, your Energy \n";
    std::cout << verb << " by approximately " << std::fixed << std::setprecision(1) << absSens << " points.\n";
}

void AppController::printCorrelationLegend() {
    std::cout << "\n------------------------------------------------\n";
    std::cout << "HOW TO READ THIS\n";
    std::cout << "------------------------------------------------\n";
    std::cout << "Positive relationship: both variables move together\n";
    std::cout << "Example: more sleep -> higher energy\n\n";
    
    std::cout << "Negative relationship: variables move in opposite directions\n";
    std::cout << "Example: more stress -> lower energy\n\n";
    
    std::cout << "Correlation strength (r):\n";
    std::cout << "Closer to 1 or -1 = stronger relationship\n";
    std::cout << "Closer to 0 = weaker relationship\n";
}
