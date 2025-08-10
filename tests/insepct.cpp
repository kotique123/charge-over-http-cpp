#include <iostream>
#include "battery.hpp"
using namespace std;
int main() {
    // Get reference to a power source.
    CFDictionaryRef pSource = BatteryAPI::getBatteryInfo();
    
    //Print raw pointer info.
    BatteryAPI::printBatteryInfo(pSource);

    //Print capacity.
    cout << "Capacity: " << BatteryAPI::getBatteryCapacity(pSource) << endl;

    //Print health;

    cout << BatteryAPI::getBatteryHealth(pSource) << endl;
    return 0;
}