#include <sstream>
#include <vector>
#include "fileManager.h"
#include <iostream>
#include <fstream>
using namespace std;

vector<string> split(string str, char delimiter) {
    vector<string> result;
    string temp;
    stringstream ss(str);

    while (getline(ss, temp, delimiter)) {
        result.push_back(temp);
    }

    return result;
}

vector<MoodEntry> FileManager::loadEntries() {
    vector<MoodEntry> entries;
    ifstream file("mood_log.csv");

    if (!file) {
        cout << "No file found." << endl;
        return entries;
    }

    string line;

    while (getline(file, line)) {
        vector<string> parts = split(line, ',');

        if (parts.size() < 4) continue;

        string timestamp = parts[0];
        int stress = stoi(parts[1]);
        int anxiety = stoi(parts[2]);

        vector<string> triggers = split(parts[3], '|');

        MoodEntry entry(stress, anxiety, triggers, timestamp);
        entries.push_back(entry);
    }

    file.close();
    return entries;
}

void FileManager::saveEntry(const MoodEntry& entry) {
    // Open the file in 'Append' mode so we don't overwrite history
    ofstream file("mood_log.csv", ios::app);

    if (!file.is_open()) {
        cerr << "Error: Could not open mood_log.csv for writing." << endl;
        return;
    }

    // 1. SERIALIZATION: Convert the vector into a single string
    string combinedTriggers = "";
    vector<string> triggers = entry.getTriggers(); // Grabbing the list from your class

    for (size_t i = 0; i < triggers.size(); ++i) {
        combinedTriggers += triggers[i];
        
        // The "Fencepost" Logic: Only add a pipe if it's NOT the last word
        if (i < triggers.size() - 1) {
            combinedTriggers += "|";
        }
    }

    // 2. WRITE THE LINE: timestamp, stress, anxiety, joined_triggers
    file << entry.getStressLevel()  << ","
         << entry.getAnxietyLevel()<< ","
         << combinedTriggers << ","
         << entry.getTimestamp()  << endl;

    file.close();
}
