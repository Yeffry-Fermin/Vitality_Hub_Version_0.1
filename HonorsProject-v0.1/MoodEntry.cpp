#include "MoodEntry.h"
#include <vector>
#include <string>
#include <ctime>
#include <sstream>

// Constructor
MoodEntry::MoodEntry(int stressLevel, int anxietyLevel, std::string note, std::vector<std::string> triggers) {
    this->stressLevel = stressLevel;
    this->anxietyLevel = anxietyLevel;
    this->triggers = triggers;
    this->note = note;
}

MoodEntry::MoodEntry(int id, int stressLevel, int anxietyLevel, std::string note, std::vector<std::string> triggers, std::string timestamp) {
    this->id = id;
    this->stressLevel = stressLevel;
    this->anxietyLevel = anxietyLevel;
    this->note = note;
    this->triggers = triggers;
    this->timestamp = timestamp;
}

// Getters
int MoodEntry::getStressLevel() const {
    return stressLevel;
}

int MoodEntry::getId() const {
    return id;
}
int MoodEntry::getAnxietyLevel() const {
    return anxietyLevel;
}

std::vector<std::string> MoodEntry::getTriggers() const {
    return triggers;
}

std::string MoodEntry::getTimestamp() const {
    return timestamp;
}

std::string MoodEntry::getNote() const {
    return note;
}

std::vector<std::string> MoodEntry::parseTriggers(const std::string& rawTriggers) {
    std::vector<std::string> result;
    std::stringstream ss(rawTriggers);
    std::string item;

    while (std::getline(ss, item, ',')) {
        // Simple trim to remove leading spaces
        item.erase(0, item.find_first_not_of(" "));
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    return result;
}
