#include "include/nmea.hxx"
#include <cstring>
#include <string>
#include <cstdlib>
#include <vector>
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
    auto vec = nmea_idents(buf);
    if (vec.empty()) return nullptr;
    vec.erase(vec.begin());
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

struct gngll_msg* nmea_gngll(const std::string& buf) {
    if (buf.empty()) return nullptr;
    if (const auto pos = buf.find('*'); pos != std::string::npos) {
        if (nmea_checksum(buf) == false) return nullptr;
    }
    auto vec = nmea_idents(buf);
    if (vec.empty()) return nullptr;
    vec.erase(vec.begin());
    if (vec.size() < 7) return nullptr;

    const auto nm = static_cast<struct gngll_msg*>(calloc(1, sizeof(struct gngll_msg)));
    if (nm == nullptr) return nullptr;

    const char latns = *(vec[1].c_str());
    const char lonew = *(vec[3].c_str());
    const char status = *(vec[5].c_str());
    const char mode = *(vec[6].c_str());
    const auto lat = nmea_clat(vec[0]);
    if (lat == nullptr) {
        free(nm);
        return nullptr;
    }
    const auto lon = nmea_clon(vec[2]);
    if (lon == nullptr) {
        free(nm);
        free(lat);
        return nullptr;
    }
    const auto tm = nmea_time(vec[4]);
    if (tm == nullptr) {
        free(nm);
        free(lat);
        free(lon);
        return nullptr;
    }

    nm->lat = lat;
    nm->latns = latns;
    nm->lon = lon;
    nm->lonew = lonew;
    nm->status = status;
    nm->mode = mode;

    return nm;
}

struct gngst_msg* nmea_gngst(const std::string& buf) {
    if (buf.empty()) return nullptr;
    if (const auto pos = buf.find('*'); pos != std::string::npos) {
        if (nmea_checksum(buf) == false) return nullptr;
    }
    auto vec = nmea_idents(buf);
    if (vec.empty()) return nullptr;
    vec.erase(vec.begin());
    if (vec.size() < 8) return nullptr;

    const auto nm = static_cast<struct gngst_msg*>(calloc(1, sizeof(struct gngst_msg)));
    if (nm == nullptr) return nullptr;

    const auto tm = nmea_time(vec[0]);
    if (tm == nullptr) { free(nm); return nullptr; }
    nm->time = tm;

    nm->rmsrange = static_cast<double>(std::stof(vec[1]));
    nm->stdmajor = static_cast<double>(std::stof(vec[2]));
    nm->stdminor = static_cast<double>(std::stof(vec[3]));
    nm->angmajor = static_cast<double>(std::stof(vec[4]));
    nm->stdlat = static_cast<double>(std::stof(vec[5]));
    nm->stdlon = static_cast<double>(std::stof(vec[6]));
    nm->stdalt = static_cast<double>(std::stof(vec[7]));

    return nm;
}

struct gnhdt_msg* nmea_gnhdt(const std::string& buf) {
    if (buf.empty()) return nullptr;
    if (const auto pos = buf.find('*'); pos != std::string::npos) {
        if (nmea_checksum(buf) == false) return nullptr;
    }
    auto vec = nmea_idents(buf);
    if (vec.empty()) return nullptr;
    vec.erase(vec.begin());
    if (vec.size() < 2) return nullptr;

    const auto nm = static_cast<struct gnhdt_msg*>(calloc(1, sizeof(struct gnhdt_msg)));
    if (nm == nullptr) return nullptr;

    nm->heading = static_cast<double>(std::stof(vec[0]));
    nm->tind = *(vec[1].c_str());

    return nm;
}

struct gnvtg_msg* nmea_gnvtg(const std::string& buf) {
    if (buf.empty()) return nullptr;
    if (const auto pos = buf.find('*'); pos != std::string::npos) {
        if (nmea_checksum(buf) == false) return nullptr;
    }
    auto vec = nmea_idents(buf);
    if (vec.empty()) return nullptr;
    vec.erase(vec.begin());
    if (vec.size() < 9) return nullptr;

    const auto nm = static_cast<struct gnvtg_msg*>(calloc(1, sizeof(struct gnvtg_msg)));
    if (nm == nullptr) return nullptr;

    const double cogtrue = std::stof(vec[0]);
    const char cogreft = *(vec[1].c_str());
    const double cogmag = std::stof(vec[2]);
    const char cogrefm = *(vec[3].c_str());
    const double sogknot = std::stof(vec[4]);
    const char sogunitn = *(vec[5].c_str());
    const double sogkph = std::stof(vec[6]);
    const char sogunitk = *(vec[7].c_str());
    const char mode = *(vec[8].c_str());

    nm->cogtrue = cogtrue;
    nm->cogreft = cogreft;
    nm->cogmag = cogmag;
    nm->cogrefm = cogrefm;
    nm->sogknot = sogknot;
    nm->sogunitn = sogunitn;
    nm->sogkph = sogkph;
    nm->sogunitk = sogunitk;
    nm->mode = mode;

    return nm;
}

struct gnzda_msg* nmea_gnzda(const std::string& buf) {
    if (buf.empty()) return nullptr;
    if (const auto pos = buf.find('*'); pos != std::string::npos) {
        if (nmea_checksum(buf) == false) return nullptr;
    }
    auto vec = nmea_idents(buf);
    if (vec.empty()) return nullptr;
    vec.erase(vec.begin());
    if (vec.size() < 6) return nullptr;

    const auto nm = static_cast<struct gnzda_msg*>(calloc(1, sizeof(struct gnzda_msg)));
    if (nm == nullptr) return nullptr;

    struct nmea_clock* tm = nmea_time(vec[0]);
    if (tm == nullptr) {
        free(nm);
        return nullptr;
    }

    const auto day = static_cast<int8_t>(std::stoi(vec[1]));
    const auto month = static_cast<int8_t>(std::stoi(vec[2]));
    const auto year = static_cast<int16_t>(std::stoi(vec[3]));

    const auto localhrs = static_cast<int8_t>(std::stoi(vec[4]));
    const auto localmin = static_cast<int8_t>(std::stoi(vec[5]));

    nm->time = tm;
    nm->day = day;
    nm->month = month;
    nm->year = year;
    nm->localhrs = localhrs;
    nm->localmin = localmin;

    return nm;
}