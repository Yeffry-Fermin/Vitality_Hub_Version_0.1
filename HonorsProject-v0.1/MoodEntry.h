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
    
    int getId() const;
    int getStressLevel() const;
    int getEnergyLevel() const;
    double getSleepHours() const;
    
    std::string getNote() const;
    std::vector<std::string> getTriggers() const;
    std::string getTimestamp() const;
    static std::vector<std::string> parseTriggers(const std::string& rawTriggers);
    
    MoodEntry(int stress, int energy, double sleep, std::string note, std::vector<std::string> triggers);
    MoodEntry(int id, int stress, int energy, double sleep, std::string note, std::vector<std::string> triggers, std::string timestamp);
};
