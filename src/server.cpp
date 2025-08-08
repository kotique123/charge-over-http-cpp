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
    res.set_content(JSONObject::getBatteryData(
        BatteryAPI::getBatteryPercentage()).dump(), "application/json");
   });
   svr.listen("0.0.0.0", 8080);
    return 0;
}
