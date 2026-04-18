#include <sstream>
#include <vector>
#include <iostream>
#include <limits>
#include "moodEntry.h"
#include "mainWindow.h"
using namespace std;



int main() {
    MainWindow controller;
    
    bool condition = true;
    int input;
    
    while(condition) {
        cout << "\n==== Vitality Hub ====\n"
             << "1. Log Mood Entry\n"
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

