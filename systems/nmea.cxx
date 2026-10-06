#include "include/nmea.hxx"
#include <cstring>
#include <string>
#include <cstdlib>
#include <vector>
#include <cstdbool>
#include <cstdint>

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
    if (const auto pos = buf.find('*'); pos != std::string::npos) buf.erase(pos);
    while (!buf.empty() && buf.front() == '*' /* Encountered the checksum */) {
        if (const auto pos = buf.find_first_of(','); pos != std::string::npos) {
            result.push_back(buf.substr(0, pos));
            buf.erase(0, pos + 1);
        } else {
            result.push_back(buf);
            break;
        }
    }
    return result;
}

bool nmea_checksum(const std::string& msg) {
    if (msg.empty()) return false;
    std::string buf = msg;
    if (const auto pos = buf.find('$'); pos != std::string::npos) buf.erase(0, pos + 1);
    unsigned int checksum = 0;
    if (const auto pos = buf.find('*'); pos != std::string::npos) {
        checksum = std::stoul(buf.substr(pos), nullptr, 16);
        buf.erase(pos);
    }
    unsigned int nw = 0;
    for (const char c : msg) {
        nw ^= static_cast<uint8_t>(c);
    }
    return checksum == nw ? true : false;
}

struct nmea_clock* nmea_time(const std::string& buf) {
    if (buf.empty()) return nullptr;
    if (buf.length() < 9) return nullptr;
    const std::string hh = buf.substr(0, 2);
    const std::string mm = buf.substr(2, 2);
    const std::string ss = buf.substr(4);
    const auto hhi = static_cast<int8_t>(std::stoi(hh));
    const auto mmi = static_cast<int8_t>(std::stoi(hh));
    const double ssf = std::stof(ss);
    const auto nmtime = static_cast<struct nmea_clock*>(calloc(1, sizeof(struct nmea_clock)));
    if (nmtime == nullptr) return nullptr;
    nmtime->hours = hhi;
    nmtime->minutes = mmi;
    nmtime->seconds = ssf;
    return nmtime;
}

struct nmea_lat* nmea_clat(const std::string& buf) {
    if (buf.empty()) return nullptr;
    if (buf.length() < 10) return nullptr;
    const std::string dd = buf.substr(0, 2);
    const std::string mm = buf.substr(2);
    const auto ddi = static_cast<int8_t>(std::stoi(dd));
    const long double mmf = std::stof(mm);
    const auto nm = static_cast<struct nmea_lat*>(calloc(1, sizeof(struct nmea_lat)));
    if (nm == nullptr) return nullptr;
    nm->lat = mmf;
    nm->latdeg = ddi;
    return nm;
}

struct nmea_lon* nmea_clon(const std::string& buf) {
    if (buf.empty()) return nullptr;
    if (buf.length() < 11) return nullptr;
    const std::string dd = buf.substr(0, 3);
    const std::string mm = buf.substr(3);
    const auto ddi = static_cast<int16_t>(std::stoi(dd));
    const long double mmf = std::stof(mm);
    const auto nm = static_cast<struct nmea_lon*>(calloc(1, sizeof(struct nmea_lon)));
    if (nm == nullptr) return nullptr;
    nm->lon = mmf;
    nm->londeg = ddi;
    return nm;
}

struct gngga_msg* nmea_gngga(const std::string& buf) {
    if (buf.empty()) return nullptr;
    if (const auto pos = buf.find('*'); pos != std::string::npos) {
            if (nmea_checksum(buf) == false) return nullptr;
    }
    const auto vec = nmea_idents(buf);
    if (vec.empty()) return nullptr;
    if (vec.size() < 14) return nullptr;

    const auto nm = static_cast<struct gngga_msg*>(calloc(1, sizeof(struct gngga_msg)));
    if (nm == nullptr) return nullptr;

    const char latns = *(vec[2].c_str());
    const char lonew = *(vec[4].c_str());
    const char altunit = *(vec[9].c_str());
    const auto quality = static_cast<int8_t>(std::stoi(vec[5]));
    const auto satcnt = static_cast<int8_t>(std::stoi(vec[6]));
    const auto stid = static_cast<int16_t>(std::stoi(vec[13]));
    const auto alt = static_cast<double>(std::stof(vec[8]));
    const auto hdop = static_cast<double>(std::stof(vec[7]));
    const auto age = static_cast<float>(std::stof(vec[12]));
    const auto lat = nmea_clat(vec[1]);
    if (lat == nullptr) {
        free(nm);
        return nullptr;
    }
    const auto lon = nmea_clon(vec[3]);
    if (lon == nullptr) {
        free(nm);
        free(lat);
        return nullptr;
    }
    const auto tm = nmea_time(vec[0]);
    if (tm == nullptr) {
        free(nm);
        free(lat);
        free(lon);
        return nullptr;
    }

    nm->time = tm;
    nm->lat = lat;
    nm->lon = lon;
    nm->latns = latns;
    nm->lonew = lonew;
    nm->alt = alt;
    nm->altunit = altunit;
    nm->hdop = hdop;
    nm->quality = quality;
    nm->satcnt = satcnt;
    nm->age = age;
    nm->stid = stid;

    return nm;
}