#include "moodEntry.h"
#include <vector>
#include <ctime>

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

