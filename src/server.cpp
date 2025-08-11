#include <iostream>
#include <httplib.h>
#include "battery.hpp"
#include "json-object.hpp"
using namespace std;

int main()
{
   httplib::Server svr;
   svr.Get("/battery", [](const httplib::Request &, httplib::Response &res)
   {
    const BatteryAPI::BatteryData data = BatteryAPI::getAllData
        (BatteryAPI::getBatteryInfo());
    res.set_content(JSONObject::batteryDataToJSON(data).dump(), "application/json");
   });
   svr.listen("0.0.0.0", 8080);
    return 0;
}
