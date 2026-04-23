#include <sstream>
#include <vector>
#include <iostream>
#include <limits>
#include "MoodEntry.h"
#include "AppController.h"

int main() {;
    DatabaseManager globalDb;
    AppController controller(globalDb); // Pass it in
    
    bool isMainMenuActive = true;
    int input;
    
    while(isMainMenuActive) {
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
                std::cout << "\n==== Analytics & Trends ====\n"
                << "1. Mood Averages (Stress & Anxiety)\n"
                << "2. Trigger Frequencies (Most Common)\n"
                << "3. Monthly/Weekly Mood Trends (TBA)\n"
                << "4. Correlation Analysis (TBA)\n"
                << "5. Back to Main Menu\n";
                
                bool isAnalyticsActive = true;
                int analyticsInput;
                
                while(isAnalyticsActive) {
                    cin >> analyticsInput;
                    
                    switch (analyticsInput) {
                        case 1: // function to handle mood averages
                            break;
                        case 2: // function to handle trigger frequencies
                            break;
                        case 3: // TBA
                            break;
                        case 4: // TBA
                            break;
                        case 5: // Go back to previous menu
                            isAnalyticsActive = false;
                            break;
                        default:
                            break;
                    }
                }
            } case 4: {
                
            } case 5: {
                isMainMenuActive = false;
                break;
            }
                
            default:
                break;
        }
    }
    return 0;
}

