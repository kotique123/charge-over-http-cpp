#include "json-object.hpp"
#include <nlohmann/json.hpp>
#include "battery.hpp"
namespace JSONObject
{
    const json batteryDataToJSON(const BatteryAPI::BatteryData& data) {
        const json jsonData {
            {"Capacity", data.Capacity},
            {"Health", data.Health},
            {"IsPluggedIn", data.isPluggedIn},
            {"TimeRemaining", data.Time}
        };
        return jsonData;
    }
    
    
}