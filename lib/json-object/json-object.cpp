#include "json-object.hpp"
#include <nlohmann/json.hpp>

json getBatteryCharge(int charge) 
{
    json battery_data = 
    {
        {"Percent", charge}
    };
    return battery_data;
}