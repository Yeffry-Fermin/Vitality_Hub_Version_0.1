#include <sstream>
#include <vector>
#include <iostream>
#include <limits>
#include "fileManager.h"
#include "moodEntry.h"
#include "mainWindow.h"
using namespace std;



int main() {
    MainWindow controller;
//    vector<MoodEntry> entries = fm.loadEntries();
//
//    for (auto& e : entries) {
//        cout << e.getTimestamp() << endl;
//        cout << "Stress: " << e.getStressLevel() << endl;
//        cout << "Triggers: ";
//
//        for (auto& t : e.getTriggers()) {
//            cout << t << " ";
//        }
//
//        cout << "\n-----------------\n";
//    }
    bool condition = true;
    int input;
    
    while(condition) {
        cout << "\n==== Vitality Hub ====\n"
             << "1. Add Mood Entry\n"
             << "2. View Trends\n"
             << "3. Exit\n"
             << "Select an option: ";
        //add input validation later for now this is ok as MVP
        cin >> input;
        switch (input) {
            case 1: {
                controller.onAddMoodEntry();
                break;
            }
            case 2: {
                cout << "Option Two Selected\n";
                break;
            }
            case 3: {
                cout << "Exiting App\n";
                condition = false;
                break;
            }
            default:
                break;
        }
    }
    return 0;
}

