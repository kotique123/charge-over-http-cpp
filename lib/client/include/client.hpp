#pragma once
#include <nlohmann/json.hpp>
#include <string>
using namespace std;
using json = nlohmann::json;

string getJSON(string server);

json parseJSON(json response);