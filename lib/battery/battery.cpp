#include "battery.hpp"
#include <fstream>
#include <stdexcept>

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
int getBatteryPercentage() {
    CFTypeRef blob = IOPSCopyPowerSourcesInfo();
    CFArrayRef sources = IOPSCopyPowerSourcesList(blob);
    if (CFArrayGetCount(sources) == 0) return -1;

    CFDictionaryRef pSource = IOPSGetPowerSourceDescription(blob, CFArrayGetValueAtIndex(sources, 0));
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
