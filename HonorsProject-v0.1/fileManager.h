#pragma once
#include <vector>
#include <string>
#include "moodEntry.h"

using namespace std;

class FileManager {
public:
    void saveEntry(const MoodEntry& entry);
    vector<MoodEntry> loadEntries();
};
