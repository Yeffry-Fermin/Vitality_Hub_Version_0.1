#pragma once
#include <string>
#include <vector>
using namespace std;

//This class responsability is simply there to to generate an entry object that
//will contain those 4 properties

class MoodEntry {
private:
    int stressLevel;
    int anxietyLevel;
    vector<string> triggers;
    string timestamp;
public:
    MoodEntry(int stressLevel, int anxietyLevel, vector<string> triggers, string timestamp);
    int getStressLevel() const;
    int getAnxietyLevel() const;
    vector<string> getTriggers() const;
    string getTimestamp() const;
};
