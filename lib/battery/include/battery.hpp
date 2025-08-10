#pragma once
#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#endif
#include <string>
namespace BatteryAPI 
{
    struct BatteryData {
       const int Capacity;
        const std::string Health;
        const bool isPluggedIn;
        const int Time;
    };
    const CFDictionaryRef getBatteryInfo();
    int getBatteryCapacity(const CFDictionaryRef pSource);
    std::string getBatteryHealth(const CFDictionaryRef pSource);
    bool isPluggedIn(const CFDictionaryRef pSource);
    int timeToFullEmpty(const CFDictionaryRef pSource, bool isPluggedIn);
    const BatteryData getAllData(const CFDictionaryRef pSource);
    void printBatteryData(const BatteryData& allData);
}