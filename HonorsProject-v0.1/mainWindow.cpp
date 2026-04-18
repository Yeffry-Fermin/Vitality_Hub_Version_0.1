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
    
    cout << "----------------\n";
    cout << "Mood Entry Selected\n";
    
    int stressLevel, anxietyLevel;
    cout << "Enter stress level from 1-10: ";
    cin >> stressLevel;
    
    cout << "Enter anxiety level from 1-10: ";
    cin >> anxietyLevel;
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter your triggers separated by commas ex: exam, rent: \n";
    std::string triggers;
    getline(cin, triggers);
    
    vector<std::string> parsedTriggers = parseTriggers(triggers);
    
    cout << "Enter a note about your mood: ";
    std::string note;
    getline(cin, note);
    cout <<"---------------------------\n";
    DatabaseManager db;
    MoodEntry newEntry(stressLevel, anxietyLevel, note, parsedTriggers);
    db.createEntry(newEntry);
}
 
//Helper function to get the time the log was created

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

