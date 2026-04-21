#pragma once
#include <vector>

class MoodEntry {
private:
    int id;
    int stressLevel;
    int anxietyLevel;
    std::string note;
    std::vector<std::string> triggers;
    std::string timestamp;

public:
    // 1. Constructor for NEW entries (No ID or timestamp yet, DB will add)
    MoodEntry(int stress, int anxiety, std::string note, std::vector<std::string> triggers);
    // 2. Constructor for EXISTING entries (Includes ID and timestamp from DB)
    MoodEntry(int id, int stress, int anxiety, std::string note, std::vector<std::string> triggers, std::string timestamp);
    
    int getId() const;
    int getStressLevel() const;
    int getAnxietyLevel() const;
    std::string getNote() const;
    std::vector<std::string> getTriggers() const;
    std::string getTimestamp() const;
    static std::vector<std::string> parseTriggers(const std::string& rawTriggers);
};
