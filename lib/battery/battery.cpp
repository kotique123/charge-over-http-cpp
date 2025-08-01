#include "battery.hpp"
#include <fstream>
#include <stdexcept>

using namespace std;

int BatteryAPI::getBatteryPercentage() 
{
    ifstream file("/sys/class/power_supply/BAT0/capacity");

    if(!file) {
        throw runtime_error("Battery capacity file is not found.");
    }
    
    int percentage;
    file >> percentage;
    return percentage;
}
