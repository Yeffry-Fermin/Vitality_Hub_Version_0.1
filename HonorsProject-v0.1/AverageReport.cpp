#include "AverageReport.h"
#include <iostream>
#include <iomanip>

AverageReport::AverageReport(float stress, float energy, float sleep, std::string msg)
: Report("MOOD REPORT"),
avgStress(stress),
avgEnergy(energy),
avgSleep(sleep),
insight(msg)
{
}

std::string AverageReport::getProgressBar(float value, int maxScale) const {
    int filledLength = static_cast<int>(value);
    
    if (filledLength > maxScale) {
        filledLength = maxScale;
    }
    
    int emptyLength = maxScale - filledLength;
    
    std::string filled(filledLength, '#');
    std::string empty(emptyLength, '-');
    
    return "[" + filled + empty + "]";
}

void AverageReport::generate() {
    
    printHeader();
    
    std::cout << "Stress: "
    << getProgressBar(avgStress)
    << " "
    << std::fixed
    << std::setprecision(1)
    << avgStress
    << "/5\n";
    
    std::cout << "Energy: "
    << getProgressBar(avgEnergy)
    << " "
    << std::fixed
    << std::setprecision(1)
    << avgEnergy
    << "/5\n";
    
    std::cout << "Sleep:   [ "
    << std::fixed
    << std::setprecision(1)
    << avgSleep
    << " Hours Average ]\n";
    
    std::cout << "\nINSIGHT: " << insight << "\n";
    
    printFooter();
}
