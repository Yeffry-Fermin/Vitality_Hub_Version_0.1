#include "MoodEntry.h"
#include <vector>
#include <string>
#include <ctime>
#include <sstream>

// Constructor for NEW entries (No ID or timestamp yet)
MoodEntry::MoodEntry(int stressLevel, int energyLevel, double sleepHours, std::string note, std::vector<std::string> triggers) {
    this->stressLevel = stressLevel;
    this->energyLevel = energyLevel;
    this->sleepHours = sleepHours;
    this->triggers = triggers;
    this->note = note;
}

// Constructor for EXISTING entries (Includes ID and timestamp from DB)
MoodEntry::MoodEntry(int id, int stressLevel, int energyLevel, double sleepHours, std::string note, std::vector<std::string> triggers, std::string timestamp) {
    this->id = id;
    this->stressLevel = stressLevel;
    this->energyLevel = energyLevel;
    this->sleepHours = sleepHours;
    this->note = note;
    this->triggers = triggers;
    this->timestamp = timestamp;
}

// --- Getters ---
int MoodEntry::getStressLevel() const {
    return stressLevel;
}

int MoodEntry::getEnergyLevel() const { // Replaced getAnxietyLevel
    return energyLevel;
}

double MoodEntry::getSleepHours() const { // New getter for Sleep
    return sleepHours;
}

int MoodEntry::getId() const {
    return id;
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

// --- Helper Functions ---
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
