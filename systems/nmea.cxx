#include "include/nmea.hxx"
#include <cstring>
#include <string>
#include <cstdlib>
#include <vector>

/*
 * GPS NMEA message utilities
 */

std::string nmea_ident(const std::string& msg) {
    // We expect the message to be already split and stripped from '\r\n'.
    const char* message = msg.c_str();
    if (message == nullptr) return "";
    if (*message == '$') {
        message++;
        if (strlen(message) >= 5) {
            const auto buf = static_cast<char *>(calloc(6, sizeof(char)));
            if (buf == nullptr) return "";
            strncpy(buf, message, 5);
            if (strcmp(buf, "GNGGA") == 0)      { free(buf); return "GNGGA"; }
            if (strcmp(buf, "GNGLL") == 0)      { free(buf); return "GNGGL"; }
            if (strcmp(buf, "GNGSA") == 0)      { free(buf); return "GNGSA"; }
            if (strcmp(buf, "GNGST") == 0)      { free(buf); return "GNGST"; }
            if (strcmp(buf, "GNHDT") == 0)      { free(buf); return "GNHDT"; }
            if (strcmp(buf, "GNRMC") == 0)      { free(buf); return "GNRMC"; }
            if (strcmp(buf, "GNVTG") == 0)      { free(buf); return "GNVTG"; }
            if (strcmp(buf, "GNZDA") == 0)      { free(buf); return "GNZDA"; }
            if (strcmp(buf + 2, "GSV") == 0)    { free(buf); return "GNGSV"; }
            free(buf);
            return "";
        } else {
            return "";
        }
    } else {
        return "";
    }
    return "";
}

std::vector<std::string> nmea_idents(const std::string& msg) {
    std::vector<std::string> result;
    if (msg.empty()) return result;
    std::string buf = msg;
    while (!buf.empty() || buf.front() == '*' /* Encountered the checksum */) {
        if (const auto pos = buf.find_first_of(','); pos != std::string::npos) {
            result.push_back(buf.substr(0, pos));
            buf.erase(0, pos + 1);
        }
    }
    return result;
}