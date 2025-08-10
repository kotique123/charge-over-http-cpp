#include "battery.hpp"
#include <CoreFoundation/CFData.h>
#include <CoreFoundation/CFDictionary.h>
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
CFDictionaryRef getBatteryInfo() {
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

int getBatteryCapacity(CFDictionaryRef pSource) {
    CFNumberRef capacity =  (CFNumberRef)CFDictionaryGetValue(pSource, CFSTR(kIOPSCurrentCapacityKey));
    int percent = -1;
    CFNumberGetValue(capacity, kCFNumberIntType, &percent);
    return percent;
}
std::string getBatteryHealth(CFDictionaryRef pSource) {
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
bool isPluggedin(CFDictionaryRef pSource) {
    
}
CFStringRef getBatteryHealthReference(CFDictionaryRef pSource)
{
    return (CFStringRef)CFDictionaryGetValue(pSource, CFSTR(kIOPSCurrentCapacityKey));
}
void printBatteryInfo(CFDictionaryRef pSource) {
    CFStringRef time = (CFStringRef)CFDictionaryGetValue(pSource, CFSTR(kIOPSTimeToFullChargeKey));
    CFStringRef cycleCount = (CFStringRef)CFDictionaryGetValue(pSource, CFSTR(kIOPSBatteryHealthKey));

    std::cout << "Time: " << time << std::endl;
    std::cout << "Cycle Count: " << cycleCount << std::endl;
}

int getBatteryPercentage() {
    CFTypeRef blob = IOPSCopyPowerSourcesInfo();
    CFArrayRef sources = IOPSCopyPowerSourcesList(blob);

    if (CFArrayGetCount(sources) == 0) return -1;

    CFDictionaryRef pSource = IOPSGetPowerSourceDescription(blob, CFArrayGetValueAtIndex(sources, 0));

    //CFRelease(blob);
    //CFRelease(sources);

    if (!pSource) return -1;

    CFNumberRef capacity = (CFNumberRef)CFDictionaryGetValue(pSource, CFSTR(kIOPSCurrentCapacityKey));

    int percent = -1;

    CFNumberGetValue(capacity, kCFNumberIntType, &percent);
    return percent;
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
