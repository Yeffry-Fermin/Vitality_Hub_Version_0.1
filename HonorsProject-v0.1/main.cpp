#include <sstream>
#include <vector>
#include <iostream>
#include <limits>
#include "MoodEntry.h"
#include "AppController.h"
#include "AnalyticsEngine.h"

int main() {;
    DatabaseManager globalDb;
    AnalyticsEngine analytics;
    AppController controller(globalDb, analytics);
    
    bool isMainMenuActive = true;
    int input;
    
    while(isMainMenuActive) {
        std::cout << "\n==== Vitality Hub ====\n"
        << "1. Log Mood Entry\n" // DONE
        << "2. View History\n" // DONE
        << "3. Analytics(TBA)\n" // WORK IN PROGRESS
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
                
                bool isAnalyticsMenuActive = true;
                int analyticsInput;
                
                while(isAnalyticsMenuActive) {
                    std::cin >> analyticsInput;
                    
                    switch (analyticsInput) {
                            
                        case 1: {
                            bool isMoodAvgMenuActive = true;
                            int moodInputOption;
                            
                            while(isMoodAvgMenuActive) {
                                std::cout << "\n==== Mood Averages ====\n"
                                << "1. Last 7 Days\n"
                                << "2. Last 30 Days\n"
                                << "3. All Time\n"
                                << "4. Back to Analytics Menu\n";
                                
                                std::cin >> moodInputOption;
                                
                                switch (moodInputOption) {
                                    case 1:
                                        // Show last 7 days averages
                                        break;
                                    case 2:
                                        // Show last 30 days averages
                                        break;
                                    case 3:
                                        // Show average from all time
                                    case 4:
                                        moodInputOption = false;
                                        break;
                                    default:
                                        break;
                                }
                            }
                            
                            break;
                        }
                            
                        case 2: // function to handle trigger frequencies
                            break;
                        case 3: // TBA
                            break;
                        case 4: // TBA
                            break;
                        case 5: // Go back to previous menu
                            isAnalyticsMenuActive = false;
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

