#pragma once
#include <vector>
#include <string>

class MoodEntry {
private:
    int id;
    int stressLevel;
    int energyLevel;     // Replaced anxietyLevel
    double sleepHours;   // Added sleepHours
    std::string note;
    std::vector<std::string> triggers;
    std::string timestamp;

public:
    // 1. Constructor for NEW entries (No ID or timestamp yet, DB will add)
    MoodEntry(int stress, int energy, double sleep, std::string note, std::vector<std::string> triggers);
    
    // 2. Constructor for EXISTING entries (Includes ID and timestamp from DB)
    MoodEntry(int id, int stress, int energy, double sleep, std::string note, std::vector<std::string> triggers, std::string timestamp);
    
    int getId() const;
    int getStressLevel() const;
    int getEnergyLevel() const;   // New getter for Energy
    double getSleepHours() const; // New getter for Sleep
    std::string getNote() const;
    std::vector<std::string> getTriggers() const;
    std::string getTimestamp() const;
    
    static std::vector<std::string> parseTriggers(const std::string& rawTriggers);
};
