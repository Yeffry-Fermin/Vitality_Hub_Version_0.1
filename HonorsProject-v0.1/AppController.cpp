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

void AppController::onViewAnalytics() {
    
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

