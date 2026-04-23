#pragma once

#include <vector>
#include <string>
#include "MoodEntry.h"

class AnalyticsEngine {
private:
public:
    float getAverageStress(const std::vector<MoodEntry>& entries) const;
    float getAverageAnxiety(const std::vector<MoodEntry>& entries) const;

};

