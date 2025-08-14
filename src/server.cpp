// Hardened battery HTTP server: timeouts, limits, robust error handling,
// safe CoreFoundation ownership, and lean JSON responses.
#include <iostream>
#include <httplib.h>
#include <string>
#include "battery.hpp"
#include "json-object.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    // Optional arg: [port] (default 8080)
    int port = 8080;
    if (argc >= 2) {
        try {
            int parsed = stoi(argv[1]);
            if (parsed <= 0 || parsed > 65535) {
                cerr << "Port out of range (1-65535)" << "\n";
                return 2;
            }
            port = parsed;
        } catch (...) {
            cerr << "Invalid port: " << argv[1] << "\n";
            return 2;
        }
    }
    httplib::Server svr;

    // Performance & safety knobs
    svr.set_read_timeout(5, 0);      // 5s read timeout
    svr.set_write_timeout(5, 0);     // 5s write timeout
    svr.set_idle_interval(0, 200000); // 200ms idle tick
    svr.set_keep_alive_max_count(100);
    svr.set_keep_alive_timeout(10);   // seconds
    svr.set_tcp_nodelay(true);
    // Even though GET has no body, keep a conservative ceiling for safety
    svr.set_payload_max_length(16 * 1024);

    // Minimal logging (method + path + status)
    svr.set_logger([](const httplib::Request& req, const httplib::Response& res) {
        cerr << req.method << " " << req.path << " -> " << res.status << "\n";
    });

    // Unified error responses in JSON
    svr.set_error_handler([](const httplib::Request& req, httplib::Response& res) {
        json body = {
            {"error", "Not Found"},
            {"path", req.path},
            {"status", 404}
        };
        res.set_content(body.dump(), "application/json; charset=utf-8");
        res.status = 404;
        res.set_header("Cache-Control", "no-store");
    });

    // Catch unhandled exceptions and respond with 500 JSON
    svr.set_exception_handler([](const httplib::Request&, httplib::Response& res, std::exception_ptr ep) {
        string what = "unexpected";
        try {
            if (ep) std::rethrow_exception(ep);
        } catch (const std::exception& e) {
            what = e.what();
        }
        json body = {
            {"error", "Internal Server Error"},
            {"detail", what},
            {"status", 500}
        };
        res.set_content(body.dump(), "application/json; charset=utf-8");
        res.status = 500;
        res.set_header("Cache-Control", "no-store");
    });

    // Liveness probe
    svr.Get("/healthz", [](const httplib::Request&, httplib::Response& res) {
        json body = {{"status", "ok"}};
        res.set_content(body.dump(), "application/json; charset=utf-8");
        res.status = 200;
        res.set_header("Cache-Control", "no-store");
    });

    svr.Get("/battery", [](const httplib::Request&, httplib::Response& res)
    {
        // Acquire battery info, build JSON, and ensure CoreFoundation object is released.
        #if defined(__APPLE__)
        const CFDictionaryRef pSource = BatteryAPI::getBatteryInfo();
        if (!pSource) {
            json body = {
                {"error", "Power source unavailable"},
                {"status", 503}
            };
            res.set_content(body.dump(), "application/json; charset=utf-8");
            res.status = 503;
            res.set_header("Cache-Control", "no-store");
            return;
        }

        try {
            const BatteryAPI::BatteryData data = BatteryAPI::getAllData(pSource);
            const json jsonData = JSONObject::batteryDataToJSON(data);
            CFRelease(pSource);

            res.set_content(jsonData.dump(), "application/json; charset=utf-8");
            res.status = 200;
            res.set_header("Cache-Control", "no-store");
        } catch (...) {
            CFRelease(pSource);
            json body = {
                {"error", "Failed to read battery information"},
                {"status", 500}
            };
            res.set_content(body.dump(), "application/json; charset=utf-8");
            res.status = 500;
            res.set_header("Cache-Control", "no-store");
        }
        #else
        json body = {
            {"error", "Battery endpoint not supported on this platform"},
            {"status", 501}
        };
        res.set_content(body.dump(), "application/json; charset=utf-8");
        res.status = 501;
        res.set_header("Cache-Control", "no-store");
        #endif
    });

    // Bind and serve
    if (!svr.listen("0.0.0.0", port)) {
        cerr << "Failed to bind on port " << port << "\n";
        return 1;
    }
    return 0;
}
