#pragma once

#include "Report.h"
#include <string>

class AverageReport : public Report {
private:
    float avgStress;
    float avgEnergy;
    float avgSleep;
    std::string insight;

    std::string getProgressBar(float value, int maxScale = 5) const;

public:
    AverageReport(float stress, float energy, float sleep, std::string msg);
    void generate() override; // intentionally replacing a virtual parent function
};
