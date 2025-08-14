#include <iostream>
#include <string>
#include <httplib.h>
#include <nlohmann/json.hpp>

using namespace std;

using json = nlohmann::json;
void printBatteryData(const json& jsonData) {
    const int capacity = jsonData.value("Capacity", -1);
    const string health = jsonData.value("Health", string("Unknown"));
    const bool isPluggedIn = jsonData.value("IsPluggedIn", false);
    const int timeRemaining = jsonData.value("TimeRemaining", -1);

    cout << "Capacity: " << capacity << endl;
    cout << "Health: " << health << endl;
    cout << "IsPluggedIn: " << (isPluggedIn ? "true" : "false") << endl;
    if (isPluggedIn) {
        cout << "Charging" << endl;
        cout << "Time to full charge: " << timeRemaining << endl;
    } else {
        cout << "Not charging" << endl;
        cout << "Time remaining on battery: " << timeRemaining << endl;
    }
}
int main(int argc, char* argv[])
{
	// Args: [host] [port]
	// host may include scheme and/or :port; explicit port arg overrides embedded port
	string host = "localhost";
	int port = 8080;

	if (argc >= 2) {
		host = argv[1];
		// Strip scheme if provided
		size_t scheme_pos = host.find("://");
		if (scheme_pos != string::npos) host = host.substr(scheme_pos + 3);
		// Drop path/query if present
		size_t slash_pos = host.find('/');
		if (slash_pos != string::npos) host = host.substr(0, slash_pos);
		// If host contains :port and no explicit port arg, parse it
		size_t colon_pos = host.rfind(':');
		if (colon_pos != string::npos) {
			string after = host.substr(colon_pos + 1);
			bool all_digits = !after.empty() && std::all_of(after.begin(), after.end(), ::isdigit);
			if (all_digits && argc < 3) {
				try {
					int parsed = stoi(after);
					if (parsed > 0 && parsed <= 65535) port = parsed; else {
						cerr << "Invalid port in host: " << after << "\n";
						return 2;
					}
					host = host.substr(0, colon_pos);
				} catch (...) {
					cerr << "Invalid port in host: " << after << "\n";
					return 2;
				}
			}
		}
	}

	if (argc >= 3) {
		try {
			int parsed = stoi(argv[2]);
			if (parsed <= 0 || parsed > 65535) {
				cerr << "Port out of range (1-65535)" << "\n";
				return 2;
			}
			port = parsed;
		} catch (...) {
			cerr << "Invalid port: " << argv[2] << "\n";
			return 2;
		}
	}

	// Client with conservative timeouts
	httplib::Client cli(host, port);
	cli.set_connection_timeout(3, 0);
	cli.set_read_timeout(5, 0);
	cli.set_write_timeout(5, 0);
	cli.set_keep_alive(true);

	httplib::Headers headers = {
		{"Accept", "application/json"}
	};

	const auto res = cli.Get("/battery", headers);
	if (!res) {
		cerr << "Unable to connect to " << host << ":" << port << "\n";
		return 3;
	}

	if (res->status != 200) {
		cerr << "Server responded with status " << res->status << "\n";
		// If server returned JSON error, try to surface message briefly
		json err = json::parse(res->body, nullptr, false);
		if (!err.is_discarded() && err.contains("error")) {
			cerr << "Error: " << err.value("error", string("")) << "\n";
		}
		return 4;
	}

	json jsonData = json::parse(res->body, nullptr, false);
	if (jsonData.is_discarded()) {
		cerr << "Failed to parse JSON response" << "\n";
		return 5;
	}

	printBatteryData(jsonData);
	return 0;
}

