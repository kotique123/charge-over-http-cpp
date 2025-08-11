#include <iostream>
#include <string>
#include <httplib.h>
#include <nlohmann/json.hpp>

using namespace std;

using json = nlohmann::json;
void printBatteryData(const json& jsonData) {
    cout << "Capacity: " << jsonData["Capacity"] << endl;
    cout << "Health: " << jsonData["Health"] << endl;
    cout << "IsPluggedIn: " << jsonData["IsPluggedIn"] << endl;
    if (jsonData["IsPluggedIn"]) {
        cout << "Charging" << endl;
        cout << "Time to full charge: " << jsonData["TimeRemaining"] << endl;
    } else {
        cout << "Not charging" << endl;
        cout << "Time remaining on battery: " << jsonData["TimeRemaining"] << endl;
    }
}
int main()
{
    httplib::Client cli("http://localhost:8080");
    const auto res = cli.Get("/battery");
    if (res->status != 200) {
        cout << "Error: " << res->status << endl;
        return 1;
    }
    const json jsonData = json::parse(res->body);
    printBatteryData(jsonData);
    return 0;
}

