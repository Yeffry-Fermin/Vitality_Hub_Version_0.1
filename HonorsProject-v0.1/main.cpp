#include <sstream>
#include <vector>
#include <iostream>
#include <limits>
#include "MoodEntry.h"
#include "AppController.h"
#include "AnalyticsEngine.h"

#include <thread> // Required for sleeping
#include <chrono> // Required for time units
#include <string>

void showLoadingAppend(int load) {
    // 1. Print the "base" word once WITHOUT a newline or \r
    std::cout << "Loading" << std::flush;

    for (int i = 0; i < load; ++i) {
        // 2. Wait for half a second 😴
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        // 3. Just add one dot to the end of the current line
        std::cout << "." << std::flush;
    }

    // 4. Move to a new line so the next output starts fresh 🏁
    std::cout << std::endl;
}

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
        << "5. Close App\n";
        //add input validation later for now this is ok as MVP
        std::cout << "Enter choice: ";
        std::cin >> input;
        switch (input) {
            case 1: {
                controller.onAddMoodEntry();
                break;
            }
            case 2: {
                showLoadingAppend(5);
                controller.onViewHistory();
                break;
            }
            case 3: {
                bool isAnalyticsMenuActive = true;
                int analyticsInput;
                
                while(isAnalyticsMenuActive) {
                    std::cout << "\n==== Analytics & Trends ====\n"
                    << "1. Mood Averages (Stress & Anxiety)\n"
                    << "2. Trigger Frequencies (Most Common)\n"
                    << "3. Monthly/Weekly Mood Trends (TBA)\n"
                    << "4. Correlation Analysis (TBA)\n"
                    << "5. Back to Main Menu\n";
                    
                    std::cout << "Enter choice: ";
                    std::cin >> analyticsInput;
                    
                    switch (analyticsInput) {
                            
                        case 1: {
                            bool isMoodAvgActive = true;
                            int moodInputOption;
                            
                            while(isMoodAvgActive) {
                                std::cout << "\n==== Mood Averages ====\n"
                                << "1. Last 7 Days\n" //DONE
                                << "2. Last 30 Days\n" //DONE
                                << "3. All Time\n" //DONE(SMALL BUG ON MENU)
                                << "4. Back to Analytics Menu\n";
                                
                                std::cout << "Enter choice: ";
                                std::cin >> moodInputOption;
                                
                                switch (moodInputOption) {
                                    case 1:
                                        showLoadingAppend(3);
                                        controller.onViewAverages(7);
                                        break;
                                    case 2:
                                        showLoadingAppend(3);
                                        controller.onViewAverages(30);
                                        break;
                                    case 3:
                                        showLoadingAppend(3);
                                        controller.onViewAverages(0);
                                        break;
                                    case 4:
                                        isMoodAvgActive = false;
                                        break;
                                    default:
                                        std::cout << "Invalid Entry, try again\n";
                                        break;
                                }
                            }
                            
                            break;
                        }
                            
                        case 2: {// function to handle trigger frequencies
                            bool isFrequentActive = true;
                            int frequentMenuInput;
                            
                            while(isFrequentActive) {
                                std::cout << "\n==== View Option ====\n"
                                << "1. Last 7 Days\n" //DONE
                                << "2. Last 30 Days\n" //DONE
                                << "3. All Time\n" //DONE(SMALL BUG ON MENU)
                                << "4. Back to Analytics Menu\n";
                                
                                std::cout << "Enter choice: ";
                                std::cin >> frequentMenuInput;
                                
                                switch (frequentMenuInput) {
                                    case 1:
                                        showLoadingAppend(4);
                                        controller.onMostFrequentTrigger(7);
                                        break;
                                    case 2:
                                        showLoadingAppend(4);
                                        controller.onMostFrequentTrigger(30);
                                        break;
                                    case 3:
                                        showLoadingAppend(4);
                                        controller.onMostFrequentTrigger(0);
                                        break;
                                    case 4:
                                        isFrequentActive = false;
                                        break;
                                    default:
                                        std::cout << "Invalid Entry, try again\n";
                                        break;
                                }
                            }
                            break;
                        }
                        case 3: {
                            bool isMoodTrendsActive = true;
                            int moodChoiceInput;
                            while(isMoodTrendsActive) {
                                std::cout << "\n==== Mood Momentum ====\n"
                                << "1. Combined Analysis 14 day (Recommended)\n"
                                << "2. Stress Trends Only\n TBA"
                                << "3. Anxiety Trends Only\n TBA"
                                << "4. Back\n";
                                
                                std::cout << "Enter choice: ";
                                std::cin >> moodChoiceInput;
                                
                                switch (moodChoiceInput) {
                                    case 1:
                                        controller.onViewMoodTrends();
                                        break;
                                        
                                    default: {
                                        isMoodTrendsActive = false;
                                        break;
                                    }
                                }
                            }
                            break;
                        }
                        case 4: // TBA
                            break;
                        case 5: {// Go back to previous menu
                            isAnalyticsMenuActive = false;
                            break;
                        }
                        default:
                            std::cout << "Invalid Entry, try again\n";
                            break;
                    }
                }
            } case 4: {
                break;
            } case 5: {
                isMainMenuActive = false;
                break;
            }
            default:
                std::cout << "Invalid Entry, try again\n";
                break;
        }
    }
    return 0;
}

