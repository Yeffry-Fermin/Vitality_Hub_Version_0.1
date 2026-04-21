#include <sstream>
#include <vector>
#include <iostream>
#include <limits>
#include "MoodEntry.h"
#include "AppController.h"

int main() {;
    DatabaseManager globalDb;
    AppController controller(globalDb); // Pass it in
    
    bool condition = true;
    int input;
    
    while(condition) {
        std::cout << "\n==== Vitality Hub ====\n"
             << "1. Log Mood Entry(Work In progress)\n"
             << "2. View History(TBA)\n"
             << "3. Analytics(TBA)\n"
             << "4. Breathing Space(Not part of MVP)\n"
             << "5. Exit \n";
        //add input validation later for now this is ok as MVP
        std::cin >> input;
        switch (input) {
            case 1: {
                controller.onAddMoodEntry();
                break;
            }
            case 2: {
                controller.onViewHistory();
                break;
            }
            case 3: {
                controller.onViewAnalytics();
                condition = false;
                break;
            }
            default:
                break;
        }
    }
    return 0;
}

