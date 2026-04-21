#pragma once

#include <vector>
#include <string>
#include "MoodEntry.h"
using namespace std;

class TrendAnalyzer {
private:
    vector<MoodEntry> logs;
public:
    TrendAnalyzer(const vector<MoodEntry>& data);

    float getAverageStress() const;
    float getAverageAnxiety() const;

    string getMostFrequentTrigger() const;
};

