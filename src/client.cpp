#include <iostream>
#include <string>
#include <httplib.h>
#include <nlohmann/json.hpp>

using namespace std;

using json = nlohmann::json;

int main()
{
    httplib::Client client("http://127.0.0.1:8080");
    client.set_read_timeout(5, 0);
    client.set_connection_timeout(5, 0);

    auto response = client.Get("/battery");
    if (!response)
    {
        std::cerr << "Request failed: no response from server" << std::endl;
        return 1;
    }

    if (response->status != 200)
    {
        std::cerr << "Request failed with status code: " << response->status << std::endl;
        return 1;
    }

    json parsed;
    try
    {
        parsed = json::parse(response->body);
    }
    catch (const json::parse_error &e)
    {
        std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
        return 1;
    }

    int charge = -1;
    if (parsed.contains("Percent") && parsed["Percent"].is_number_integer())
    {
        charge = parsed["Percent"].get<int>();
    }
    else
    {
    
        std::cerr << "Unexpected JSON format: missing integer field 'Percent'" << std::endl;
        return 1;
    }

    std::cout << "Battery charge: " << charge << std::endl;
    return 0;
}

