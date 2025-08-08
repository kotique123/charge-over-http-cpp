#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace JSONObject
{
json getBatteryData(int charge);
}

