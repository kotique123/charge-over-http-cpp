#pragma once
#if defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#endif
#include <string>
namespace BatteryAPI 
{
    int getBatteryPercentage();
    CFDictionaryRef getBatteryInfo();
    void printBatteryInfo(CFDictionaryRef pSource);
    int getBatteryCapacity(CFDictionaryRef pSource);
    std::string getBatteryHealth(CFDictionaryRef pSource);
}