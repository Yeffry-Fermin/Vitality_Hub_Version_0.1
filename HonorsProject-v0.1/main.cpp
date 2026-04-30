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

int main() {
    DatabaseManager globalDb;
    AnalyticsEngine analytics;
    AppController controller(globalDb, analytics);
    
    bool isMainMenuActive = true;
    int input;
    
    while(isMainMenuActive) {
        // --- CHANGED: Cleaned up numbering to 1-4 ---
        std::cout << "\n==== Vitality Hub ====\n"
        << "1. Log Mood Entry\n"
        << "2. View History\n"
        << "3. Analytics Engine\n"
        << "4. Close App\n";
        
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
                    // --- CHANGED: Updated menu text for the Triad Model ---
                    std::cout << "\n==== ANALYTICS & INSIGHTS ====\n";
                    std::cout << "1. Resource Baselines (System Averages)\n";
                    std::cout << "2. Trigger Impact & Pattern Finder\n";
                    std::cout << "3. Vitality Momentum (14-Day View)\n";
                    std::cout << "4. System Linkages (Correlation Report)\n";
                    std::cout << "5. Return to Dashboard\n";
                    std::cout << "Selection: ";
                    
                    std::cout << "Enter choice: ";
                    std::cin >> analyticsInput;
                    
                    switch (analyticsInput) {
                            
                        case 1: {
                            bool isMoodAvgActive = true;
                            int moodInputOption;
                            
                            while(isMoodAvgActive) {
                                std::cout << "\n==== Mood Averages ====\n"
                                << "1. Last 7 Days\n"
                                << "2. Last 30 Days\n"
                                << "3. All Time\n"
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
                            
                        case 2: {
                            bool isFrequentActive = true;
                            int frequentMenuInput;
                            
                            while(isFrequentActive) {
                                std::cout << "\n==== View Option ====\n"
                                << "1. Last 7 Days\n"
                                << "2. Last 30 Days\n"
                                << "3. All Time\n"
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
                                // --- CHANGED: Updated text to match new metrics ---
                                std::cout << "\n==== Mood Momentum ====\n"
                                << "1. Combined Triad Analysis (14-day)\n"
                                << "2. Stress Trends Only (TBA)\n"
                                << "3. Energy Trends Only (TBA)\n"
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
                            std::cout << "\nFeature coming soon!\n";
                            break;
                        case 5: {
                            isAnalyticsMenuActive = false;
                            break;
                        }
                        default:
                            std::cout << "Invalid Entry, try again\n";
                            break;
                    }
                }
                // --- BUG FIX: Added missing break to prevent the app from closing! ---
                break;
            }
            case 4: {
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
