//
#pragma once
#include <nlohmann/json.hpp>
#include "battery.hpp"
using json = nlohmann::json;
namespace JSONObject
{
const json batteryDataToJSON(const BatteryAPI::BatteryData& data);
}

