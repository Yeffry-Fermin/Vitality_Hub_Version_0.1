#pragma

#include "fileManager.h"
#include "trendAnalyzer.h"
#include <string>

class MainWindow {
private:
    // These are the "Sub-Bosses" that MainWindow manages
    FileManager fileManager;
    TrendAnalyzer* analyzer; // We use a pointer because data might change
//    BreathingCoach coach;
    string getTimestamp();

public:
    // Constructor: Sets up the file manager and other components
    MainWindow();
    
    // Destructor: Clean up the analyzer pointer if needed
//    ~MainWindow();

    // These represent the main "Choices" in your app's logic
    void onAddMoodEntry();   // Logic to gather input and save to file
    void onTrendsClicked(); // Logic to trigger the analyzer and show results
    void onCalmRoomClicked(); // Logic to start the breathing coach
    
    vector<string> parseTriggers(string input);
};

