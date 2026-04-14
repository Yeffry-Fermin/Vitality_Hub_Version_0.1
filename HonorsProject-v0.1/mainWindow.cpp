#include "mainWindow.h"
#include "moodEntry.h"
#include <iostream>
#include <string>
#include <chrono>   // For the clock
#include <ctime>    // For the calendar conversion
#include <iomanip>  // For the time formatting (put_time)
#include <sstream>  // For the string "bucket" (ostringstream)
using namespace std;


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

    cout << "Enter your triggers Ex. exam, rent: \n";
    string triggers;
    getline(cin, triggers);
    
    vector<string> parsedStrings = parseTriggers(triggers);
    cout << "Entry successful!\nPress enter to return...";

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    
    string currentTime = getTimestamp(); // Nice and clean!

    MoodEntry newEntry(stressLevel, anxietyLevel, parsedStrings, currentTime);
    
    FileManager fm;
    fm.saveEntry(newEntry);
}
 
//Helper function to get the time the log was created

vector<string> MainWindow::parseTriggers(string input) {
    
    vector<string> result;
    string temp;
    stringstream ss(input);

    while (getline(ss, temp, ',')) {
        // optional: remove leading space
        if (!temp.empty() && temp[0] == ' ')
            temp.erase(0, 1);

        result.push_back(temp);
    }

    return result;
}

string MainWindow::getTimestamp() {
    time_t now = time(nullptr);
        char buf[20];
        // This formats the time directly into the 'buf' character array
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", localtime(&now));
        return string(buf);
}
