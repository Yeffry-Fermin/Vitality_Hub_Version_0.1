#include "moodEntry.h"
#include <ctime>

// Constructor
MoodEntry::MoodEntry(int stressLevel, int anxietyLevel, vector<string> triggers, string timestamp) {
    this->stressLevel = stressLevel;
    this->anxietyLevel = anxietyLevel;
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

vector<string> MoodEntry::getTriggers() const {
    return triggers;
}

string MoodEntry::getTimestamp() const {
    return timestamp;
}
