#include "battery.hpp"
#include <iostream>

using namespace std;

int main() {
    cout << BatteryAPI::getBatteryPercentage() << endl;
    return 0;
}