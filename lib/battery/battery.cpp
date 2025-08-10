#include "battery.hpp"
#include <CoreFoundation/CFData.h>
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFNumber.h>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <string>
using namespace std;
namespace BatteryAPI {
#if defined(__linux__)
int getBatteryPercentage() 
{
    ifstream file("/sys/class/power_supply/BAT0/capacity");

    if(!file) {
        throw runtime_error("Battery capacity file is not found.");
    }
    
    int percentage;
    file >> percentage;
    return percentage;
}
#elif(__APPLE__)
#include <IOkit/ps/IOPowerSources.h>
#include <IOKit/ps/IOPSKeys.h>
#include <CoreFoundation/CoreFoundation.h>

// TODO #1: Split getBatteryPercentage into several functions:
// 0. getBatteryInfo()
// 1. getBatteryPercentage()
// 2. getBatteryStatus()
// 3. getBatteryTime()
// 4. getBatteryHealth()
// 5. getBatteryCycleCount()
const CFDictionaryRef getBatteryInfo() {
    CFTypeRef blob = IOPSCopyPowerSourcesInfo();

    CFArrayRef sources = IOPSCopyPowerSourcesList(blob);

    CFDictionaryRef pSource = IOPSGetPowerSourceDescription(blob, CFArrayGetValueAtIndex(sources, 0));

    // Create a mutable copy of the dictionary to make it independent
    CFMutableDictionaryRef mutableCopy = CFDictionaryCreateMutableCopy(kCFAllocatorDefault, 0, pSource);
    
    // Now we can safely release the original objects
    CFRelease(blob);
    CFRelease(sources);
    
    // Return the independent copy (caller must release this)
    return mutableCopy;
}

int getBatteryCapacity(const CFDictionaryRef pSource) {
    CFNumberRef capacity =  (CFNumberRef)CFDictionaryGetValue(pSource, CFSTR(kIOPSCurrentCapacityKey));
    int percent = -1;
    CFNumberGetValue(capacity, kCFNumberIntType, &percent);
    return percent;
}
std::string getBatteryHealth(const CFDictionaryRef pSource) {
    CFStringRef health = (CFStringRef)CFDictionaryGetValue(pSource, CFSTR(kIOPSBatteryHealthKey));
    
    if (!health) {
        return "Not available";
    }
    
    // Convert CFString to C string for display
    CFIndex length = CFStringGetLength(health);
    CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
    char* healthStr = new char[maxSize];
    
    if (CFStringGetCString(health, healthStr, maxSize, kCFStringEncodingUTF8)) {
        std::string result(healthStr);
        delete[] healthStr;  // ✅ Free the allocated memory!
        return result;
    } else {
        delete[] healthStr;  // ✅ Free memory even on failure
        return "Unable to retrieve";
    }
}
bool isPluggedIn(const CFDictionaryRef pSource) {
    CFStringRef powerSourceState = (CFStringRef)CFDictionaryGetValue(pSource, CFSTR(kIOPSPowerSourceStateKey));
    
    if (!powerSourceState) {
        return false;
    }
    // Check if the power source state indicates AC power (plugged in)
    return CFStringCompare(powerSourceState, CFSTR(kIOPSACPowerValue), 0) == kCFCompareEqualTo;
}

int timeToFullEmpty(const CFDictionaryRef pSource, bool isPluggedIn) {
    if (isPluggedIn) {
        CFNumberRef time = (CFNumberRef)CFDictionaryGetValue(pSource, CFSTR(kIOPSTimeToFullChargeKey));
        if (time) {
            int minutes = -1;
            CFNumberGetValue(time, kCFNumberIntType, &minutes);
            return minutes;
        }
    } else {
        CFNumberRef time = (CFNumberRef)CFDictionaryGetValue(pSource, CFSTR(kIOPSTimeToEmptyKey));
        if (time) {
            int minutes = -1;
            CFNumberGetValue(time, kCFNumberIntType, &minutes);
            return minutes;
        }
    }
    return -1;
}
const BatteryData getAllData(const CFDictionaryRef pSource) {

    const BatteryData allData = {
            getBatteryCapacity(pSource),
            getBatteryHealth(pSource),
            isPluggedIn(pSource),
            timeToFullEmpty(pSource, allData.isPluggedIn)

    };
    return allData;
}
void printBatteryData(const BatteryData &allData) {
    cout << allData.Capacity << endl;
    cout << allData.Health << endl;
    cout << allData.isPluggedIn << endl;
    cout << allData.Time << endl;
}
#elif(_WIN32)
int getBatteryPercentage() {
    SYSTEM_POWER_STATUS sps;
    if (GetSystemPowerStatus(&sps)) {
        return sps.BatteryLifePercent; // 0–100
    }
    return -1;
}
#endif
}
