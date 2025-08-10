#include <CoreFoundation/CFDictionary.h>
#include <iostream>
#include "battery.hpp"
using namespace std;
int main() {
    // Get reference to a power source.
    const CFDictionaryRef pSource = BatteryAPI::getBatteryInfo();
    const BatteryAPI::BatteryData data = BatteryAPI::getAllData(pSource);
    BatteryAPI::printBatteryData(data);
}