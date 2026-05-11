#pragma once
#include <iostream>
#include <string>

class Report {
protected:
    std::string title;

public:
    Report(std::string t) : title(t) {}

    // virtual allows classes to replace this function with their own version[note]
    virtual ~Report() {}
    virtual void generate() = 0;

    void printHeader() const {
        std::cout << "\n================================\n";
        std::cout << "   " << title << "\n";
        std::cout << "================================\n";
    }

    void printFooter() const {
        std::cout << "================================\n";
    }
};
