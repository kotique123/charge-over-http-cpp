#include "battery.hpp"
#include <iostream>
#include <httplib.h>
#include "json-object.hpp"
using namespace std;

int main()
{
    httplib::Server svr;

    svr.Get("/battery", [](const httplib::Request&, httplib::Response& res)
    {
        try
        { 
            json battery_data = getBatteryData(BatteryAPI::getBatteryPercentage());
            res.set_content(to_string(battery_data), "text/plain");
        }
        catch (const std::exception& e)
        {
            res.status = 500;
            res.set_content(e.what(), "text/plain");
        }
    });

    cout << "Server started at http:://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
}
