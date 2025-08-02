#include "json-object.hpp"
#include "battery.hpp"
#include <iostream>
auto charge = BatteryAPI::getBatteryPercentage();


int main() {
    auto charge = BatteryAPI::getBatteryPercentage();
    json battery_info = getBatteryCharge(charge);
    std::cout << battery_info;
    return 0;
}