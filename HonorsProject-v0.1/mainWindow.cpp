#include <string>
#include "mainWindow.h"
#include "moodEntry.h"
#include <iostream>
#include <limits>
#include <vector>
#include <chrono>   // For the clock
#include <ctime>    // For the calendar conversion
#include <iomanip>  // For the time formatting (put_time)
#include <sstream>  // For the string "bucket" (ostringstream)
#include "databaseManager.h"


//When user clicks Save Log sample of getting data and saving
MainWindow::MainWindow(){
    
}
void MainWindow::onAddMoodEntry() {
    
    std::cout << "----------------\n";
    std::cout << "Mood Entry Selected\n";
    
    int stressLevel = getValidInt("Enter stress level from 1-10: ", 1, 10);
    int anxietyLevel = getValidInt("Enter anxiety level from 1-10: ", 1, 10);
    
    std::cout << "Enter your triggers separated by commas ex: exam, rent: \n";
    std::string triggers;
    getline(cin, triggers);
    
    vector<std::string> parsedTriggers = parseTriggers(triggers);
    
    std::cout << "Enter a note about your mood: ";
    std::string note;
    getline(cin, note);
    std::cout <<"---------------------------\n";
    
    // Create and save db instance entry
    DatabaseManager db;
    MoodEntry newEntry(stressLevel, anxietyLevel, note, parsedTriggers);
    db.createEntry(newEntry);
}
 
//Helper function to parse strings into vector commas
vector<std::string> MainWindow::parseTriggers(std::string input) {
    
    vector<std::string> result;
    std::string temp;
    std::stringstream ss(input);

    while (getline(ss, temp, ',')) {
        // optional: remove leading space
        if (!temp.empty() && temp[0] == ' ')
            temp.erase(0, 1);

        result.push_back(temp);
    }

    return result;
}

//Helper function to help me with getting correct prompt values
int MainWindow::getValidInt(string prompt, int min, int max) {
    int value;
    bool isValid;

    do {
        isValid = true; // 1. Start by assuming the input is good
        std::cout << prompt << endl;
        
        // 2. Try to read the number AND check the range
        // If the read fails OR the number is out of bounds...
        if (!(std::cin >> value) || value < min || value > max) {
            isValid = false;
            
            std::cin.clear(); // Reset the safety breaker
            std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the pipe
            
            cerr << "Error: Please enter a number between " << min << " and " << max << "." << endl;
        }
    } while (!isValid); // 4. If NOT valid, go back to the top
    
    return value;
}
