#include <string>
#include <iostream>
#include <limits>
#include <vector>
#include <chrono>   // For the clock
#include <ctime>    // For the calendar conversion
#include <iomanip>  // For the time formatting (put_time)
#include <sstream>  // For the string "bucket" (ostringstream)

#include "MoodEntry.h"
#include "AppController.h"
#include "DatabaseManager.h"
#include "AnalyticsEngine.h"

// Updated constructor to handle both dependencies
AppController::AppController(DatabaseManager& dbInstance, AnalyticsEngine& analyticsInstance)
    : db(dbInstance), analytics(analyticsInstance)
{
}

void AppController::onAddMoodEntry() {
    
    std::cout << "----------------\n";
    std::cout << "Mood Entry Selected\n";
    
    int stressLevel = getValidInt("Enter stress level from 1-10: ", 1, 10);
    int anxietyLevel = getValidInt("Enter anxiety level from 1-10: ", 1, 10);
    
    std::string triggers;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    do {
        std::cout << "Enter your triggers separated by commas ex: exam, rent: \n";
        getline(std::cin, triggers);
    } while (triggers.length() < 2);
    
    std::vector<std::string> parsedTriggers = MoodEntry::parseTriggers(triggers);
    
    std::cout << "Enter a note about your mood: ";
    std::string note;
    getline(std::cin, note);
    std::cout <<"---------------------------\n";
    
    // Create and save db instance entry
    MoodEntry newEntry(stressLevel, anxietyLevel, note, parsedTriggers);
    db.createEntry(newEntry);
}

void AppController::onViewHistory() {
    std::vector<MoodEntry> history = db.getEntries(0);
    
    if (history.empty()) {
        std::cout << "\nNo logs found yet. Start by adding your first entry!\n";
        return;
    }
    
    // 1. Print the Header
    std::cout << std::left << std::setw(4)  << "ID"
    << std::setw(22) << "Date"
    << std::setw(10) << "Stress"
    << std::setw(10) << "Anxiety"
    << "Note / Triggers" << std::endl;
    
    std::cout << std::string(80, '-') << std::endl; // A simple separator line
    
    for(const auto& entry: history) {
        std::cout << std::left << std::setw(4)  << entry.getId()
        << std::setw(22) << entry.getTimestamp()
        << std::setw(10) << entry.getStressLevel()
        << std::setw(10) << entry.getAnxietyLevel();
        
        std::vector<std::string> triggers = entry.getTriggers();
        int lastIndex = (int)triggers.size() - 1;
        
        for (int i = 0; i < triggers.size(); i++) {
            std::cout << triggers[i];                            // 3. Print the word
            if (i < lastIndex) {                                 // 4. Your logic!
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
    float avgAnxiety = analytics.getAverageAnxiety(entries);
    

    std::cout << "\n================================" << std::endl;
    std::cout << "   MOOD REPORT (Last " << option << " Days)" << std::endl;
    std::cout << "================================" << std::endl;

    std::cout << "Stress:  " << getProgressBar(avgStress) << " " << std::fixed
              << std::setprecision(2) <<avgStress << "/10" << std::endl;
    
    std::cout << "Anxiety: " << getProgressBar(avgAnxiety) << " " << std::fixed
              << std::setprecision(2) << avgAnxiety << "/10" << std::endl;
    std::cout << "================================" << std::endl;
    
}
void AppController::onMostFrequentTrigger(int option) {
    std::vector<MoodEntry> entries = db.getEntries(option);
    
    auto insight = analytics.getTriggerInsights(entries);
    
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "        TOP MOOD TRIGGER REPORT       " << std::endl;
    std::cout << "========================================" << std::endl;

    // Check if a trigger was actually found
    if (insight.trigger == "None") {
        std::cout << " No data available yet. Keep logging! 📝" << std::endl;
    } else {
        std::cout << " Trigger Name:   " << insight.trigger << std::endl;
        std::cout << " Frequency:      " << insight.frequency << " times" << std::endl;
        
        // Format floats to 1 decimal place
        std::cout << std::fixed << std::setprecision(1);
        std::cout << " Avg. Stress:    " << insight.avgStress << " / 10" << std::endl;
        std::cout << " Avg. Anxiety:   " << insight.avgAnxiety << " / 10" << std::endl;
    }

    std::cout << "========================================\n" << std::endl;

}

void AppController::onViewMoodTrends() {
    
    std::vector<MomentumPoint> points = db.getMoodMomentum();
    
    std::cout << "\n==== MOOD MOMENTUM (Last 14 Logs) ====\n";
    std::cout << "DATE       [ STRESS ]   |   [ ANXIETY ]\n";
    
    // We iterate backwards to show the most recent data at the bottom
    for (auto it = points.rbegin(); it != points.rend(); ++it) {
        std::cout << it->date << " | ";

        // Draw Stress Bar
        int sWidth = (int)it->rollingStress;
        for(int i=0; i<10; i++) std::cout << (i < sWidth ? "█" : " ");

        std::cout << " | ";

        // Draw Anxiety Bar
        int aWidth = (int)it->rollingAnxiety;
        for(int i=0; i<10; i++) std::cout << (i < aWidth ? "█" : " ");

        std::cout << "\n";
    }
}





//Helper function to help me with getting correct prompt values
int AppController::getValidInt(std::string prompt, int min, int max) {
    int value;
    bool isValid;
    
    do {
        isValid = true; // 1. Start by assuming the input is good
        std::cout << prompt << std::endl;
        
        // 2. Try to read the number AND check the range If the read fails OR the number is out of bounds...
        if (!(std::cin >> value) || value < min || value > max) {
            isValid = false;
            
            std::cin.clear(); // Reset the safety breaker
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the pipe
            
            std::cerr << "Error: Please enter a number between " << min << " and " << max << "." << std::endl;
        }
    } while (!isValid); // 4. If NOT valid, go back to the top
    
    return value;
}

std::string AppController::getProgressBar(float value, int maxScale) const {
    int filledLength = static_cast<int>(value); // Round the float to an int
    int emptyLength = maxScale - filledLength;

    // Create the "filled" part (e.g., #######)
    std::string filled(filledLength, '#');
    
    // Create the "empty" part (e.g., ---)
    std::string empty(emptyLength, '-');

    return "[" + filled + empty + "]";
}
