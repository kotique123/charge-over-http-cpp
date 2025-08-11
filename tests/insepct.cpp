#include <CoreFoundation/CFDictionary.h>
#include <iostream>
#include "battery.hpp"
#include "json-object.hpp"
using json = nlohmann::json;
using namespace std;
int main() {
    // Get reference to a power source.
    const CFDictionaryRef pSource = BatteryAPI::getBatteryInfo();
    const BatteryAPI::BatteryData data = BatteryAPI::getAllData(pSource);
    BatteryAPI::printBatteryData(data);
    const json jsonData = JSONObject::batteryDataToJSON(data);
    cout << jsonData.dump(4) << endl;
    return 0;
}