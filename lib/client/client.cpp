#include "client.hpp"
#include <httplib.h>

auto getJSON(string server) {
    httplib::Client cli(server);
    auto res = cli.Get("/battery");
    return res;
}

string parseJSON(json response) {
    json data = json::parse(response);
}