#include "include/nmea.hxx"
#include <cstring>
#include <string>
#include <cstdlib>
#include <vector>
#include <cstdint>
#include <iostream>
#include <ostream>

/*
 * GPS NMEA message utilities
 */

static int stoi_s(const std::string& s) {
    try {
        return std::stoi(s);
    } catch (...) {
        return 0;
    }
}

static float stof_s(const std::string& s) {
    try {
        return std::stof(s);
    } catch (...) {
        return 0.0f;
    }
}

static double stod_s(const std::string& s) {
    try {
        return std::stod(s);
    } catch (...) {
        return 0.0;
    }
}

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
            if (strcmp(buf + 2, "GGA") == 0)    { free(buf); return "GNGGA"; }
            if (strcmp(buf + 2, "GLL") == 0)    { free(buf); return "GNGLL"; }
            if (strcmp(buf + 2, "GSA") == 0)    { free(buf); return "GNGSA"; }
            if (strcmp(buf + 2, "GST") == 0)    { free(buf); return "GNGST"; }
            if (strcmp(buf + 2, "HDT") == 0)    { free(buf); return "GNHDT"; }
            if (strcmp(buf + 2, "RMC") == 0)    { free(buf); return "GNRMC"; }
            if (strcmp(buf + 2, "VTG") == 0)    { free(buf); return "GNVTG"; }
            if (strcmp(buf + 2, "ZDA") == 0)    { free(buf); return "GNZDA"; }
            if (strcmp(buf + 2, "GSV") == 0)    { free(buf); return "GNGSV"; }
            if (strcmp(buf + 2, "TXT") == 0)    { free(buf); return "GNTXT"; } // ex. u-blox AG boot messages
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
    const auto star = msg.find('*');
    const std::string buf = msg.substr(0, star);
    size_t start = 0;

    while (true) {
        const auto pos = buf.find(',', start);
        if (pos == std::string::npos) {
            result.push_back(buf.substr(start));
            break;
        }
        result.push_back(buf.substr(start, pos - start));
        start = pos + 1;
    }
    return result;
}

bool nmea_checksum(const std::string& msg) {
    if (msg.empty()) return false;
    auto buf = msg.c_str();
    if (*buf != '$') return false;
    buf++;
    unsigned int current = 0;
    while (*buf != '\0' && *buf != '*') {
        const auto c = *buf;
        current ^= static_cast<uint8_t>(c);
        buf++;
    }
    if (*buf == '*') {
        buf++;
        const auto provided = static_cast<unsigned int>(strtoul(buf, nullptr, 16));
        return provided == current;
    }
    return true;
}

struct nmea_clock* nmea_time(const std::string& buf) {
    const auto nmtime = static_cast<struct nmea_clock*>(calloc(1, sizeof(struct nmea_clock)));
    if (nmtime == nullptr) return nullptr;

    if (buf.length() < 6) {
        nmtime->hours = 0;
        nmtime->minutes = 0;
        nmtime->seconds = 0;
        return nmtime;
    }
    const std::string hh = buf.substr(0, 2);
    const std::string mm = buf.substr(2, 2);
    const std::string ss = buf.substr(4);
    const auto hhi = static_cast<int8_t>(stoi_s(hh));
    const auto mmi = static_cast<int8_t>(stoi_s(mm));
    const double ssf = stof_s(ss);
    nmtime->hours = hhi;
    nmtime->minutes = mmi;
    nmtime->seconds = ssf;
    return nmtime;
}

struct nmea_lat* nmea_clat(const std::string& buf) {
    const auto nm = static_cast<struct nmea_lat*>(calloc(1, sizeof(struct nmea_lat)));
    if (nm == nullptr) return nullptr;

    if (buf.length() < 4) {
        nm->lat = 0;
        nm->latdeg = 0;
        return nm;
    }
    const std::string dd = buf.substr(0, 2);
    const std::string mm = buf.substr(2);
    const auto ddi = static_cast<int8_t>(stoi_s(dd));
    const long double mmf = stod_s(mm);
    nm->lat = mmf;
    nm->latdeg = ddi;
    return nm;
}

struct nmea_lon* nmea_clon(const std::string& buf) {
    const auto nm = static_cast<struct nmea_lon*>(calloc(1, sizeof(struct nmea_lon)));
    if (nm == nullptr) return nullptr;

    if (buf.length() < 5) {
        nm->lon = 0;
        nm->londeg = 0;
        return nm;
    }
    const std::string dd = buf.substr(0, 3);
    const std::string mm = buf.substr(3);
    const auto ddi = static_cast<int16_t>(stoi_s(dd));
    const long double mmf = stod_s(mm);
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
    const auto quality = static_cast<int8_t>(stoi_s(vec[5]));
    const auto satcnt = static_cast<int8_t>(stoi_s(vec[6]));
    const auto stid = static_cast<int16_t>(stoi_s(vec[13]));
    const auto alt = stod_s(vec[8]);
    const auto hdop = stod_s(vec[7]);
    const auto age = stof_s(vec[12]);
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
    nm->time = tm;

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

    nm->rmsrange = stod_s(vec[1]);
    nm->stdmajor = stod_s(vec[2]);
    nm->stdminor = stod_s(vec[3]);
    nm->angmajor = stod_s(vec[4]);
    nm->stdlat = stod_s(vec[5]);
    nm->stdlon = stod_s(vec[6]);
    nm->stdalt = stod_s(vec[7]);

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

    nm->heading = static_cast<double>(stod_s(vec[0]));
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

    const double cogtrue = stof_s(vec[0]);
    const char cogreft = *(vec[1].c_str());
    const double cogmag = stof_s(vec[2]);
    const char cogrefm = *(vec[3].c_str());
    const double sogknot = stof_s(vec[4]);
    const char sogunitn = *(vec[5].c_str());
    const double sogkph = stof_s(vec[6]);
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

    const auto day = static_cast<int8_t>(stoi_s(vec[1]));
    const auto month = static_cast<int8_t>(stoi_s(vec[2]));
    const auto year = static_cast<int16_t>(stoi_s(vec[3]));

    const auto localhrs = static_cast<int8_t>(stoi_s(vec[4]));
    const auto localmin = static_cast<int8_t>(stoi_s(vec[5]));

    nm->time = tm;
    nm->day = day;
    nm->month = month;
    nm->year = year;
    nm->localhrs = localhrs;
    nm->localmin = localmin;

    return nm;
}

struct gngsa_msg* nmea_gngsa(const std::string& buf) {
    if (buf.empty()) return nullptr;
    if (const auto pos = buf.find('*'); pos != std::string::npos) {
        if (nmea_checksum(buf) == false) return nullptr;
    }
    auto vec = nmea_idents(buf);
    if (vec.empty()) return nullptr;
    vec.erase(vec.begin());
    if (vec.size() < 17) return nullptr;

    const auto nm = static_cast<struct gngsa_msg*>(calloc(1, sizeof(struct gngsa_msg)));
    if (nm == nullptr) return nullptr;

    nm->modeop = static_cast<char>(*(vec[0].c_str()));
    nm->modenav = static_cast<char>(*(vec[1].c_str()));

    for (auto i = 2; i <= 13; i++) {
        nm->ids[i - 2] = static_cast<int8_t>(stoi_s(vec[i]));
    }

    nm->pdop = static_cast<double>(stod_s(vec[14]));
    nm->hdop = static_cast<double>(stod_s(vec[15]));
    nm->vdop = static_cast<double>(stod_s(vec[16]));
    nm->gnssid = vec.size() > 17 ? static_cast<int8_t>(stoi_s(vec[17])) : 0;

    return nm;
}

/*
 * some systems might not include navstat in gnrmc,
 * use the NMEA_NAVSTAT if your system provides it but it is not necessary
 */
struct gnrmc_msg* nmea_gnrmc(const std::string& buf) {
    if (buf.empty()) return nullptr;
    if (const auto pos = buf.find('*'); pos != std::string::npos) {
        if (nmea_checksum(buf) == false) return nullptr;
    }
    auto vec = nmea_idents(buf);
    if (vec.empty()) return nullptr;
    vec.erase(vec.begin());
#ifndef NMEA_NAVSTAT
    if (vec.size() < 12) return nullptr;
#else
    if (vec.size() < 13) return nullptr;
#endif

    const auto nm = static_cast<struct gnrmc_msg*>(calloc(1, sizeof(struct gnrmc_msg)));
    if (nm == nullptr) return nullptr;

    const auto tm = nmea_time(vec[0]);
    if (tm == nullptr) {
        free(nm);
        return nullptr;
    }

    const auto lat = nmea_clat(vec[2]);
    if (lat == nullptr) {
        free(nm);
        free(tm);
        return nullptr;
    }
    const auto lon = nmea_clon(vec[4]);
    if (lon == nullptr) {
        free(nm);
        free(tm);
        free(lat);
        return nullptr;
    }

    const char latns = *(vec[3].c_str());
    const char lonew = *(vec[5].c_str());
    const char magvarew = *(vec[10].c_str());
    const char mode = *(vec[11].c_str());
#ifndef NMEA_NAVSTAT
    constexpr char navstat = 'V';
#else
    const char navstat = *(vec[12].c_str());
#endif
    const char status = *(vec[1].c_str());

    const auto speed = stod_s(vec[6]);
    const auto course = stod_s(vec[7]);
    const auto magvar = stod_s(vec[9]);

    const std::string dt = vec[8];
    int8_t day = 0;
    int8_t month = 0;
    int8_t year = 0;
    if (dt.length() >= 6) {
        day = static_cast<int8_t>(stoi_s(dt.substr(0, 2)));
        month = static_cast<int8_t>(stoi_s(dt.substr(2, 2)));
        year = static_cast<int8_t>(stoi_s(dt.substr(4, 2)));
    }

    nm->time = tm;
    nm->status = status;
    nm->lat = lat;
    nm->lon = lon;
    nm->latns = latns;
    nm->lonew = lonew;
    nm->speed = speed;
    nm->course = course;
    nm->mode = mode;
    nm->navstat = navstat;
    nm->magvarew = magvarew;
    nm->magvar = magvar;
    nm->day = day;
    nm->month = month;
    nm->year = year;

    return nm;
}

struct gxgsv_msg* nmea_gxgsv(const std::string& buf) {
    if (buf.empty()) return nullptr;
    if (const auto pos = buf.find('*'); pos != std::string::npos) {
        if (nmea_checksum(buf) == false) return nullptr;
    }
    auto vec = nmea_idents(buf);
    if (vec.empty()) return nullptr;
    if (vec[0].length() < 2) return nullptr;
    const char* mn = vec.begin()->c_str();
    const auto callv = static_cast<char*>(calloc(3, sizeof(char)));
    if (callv == nullptr) return nullptr;

    memcpy(callv, mn + 1, 2 * sizeof(char));
    callv[2] = '\0';
    vec.erase(vec.begin());
    if (vec.size() < 3) {
        free(callv);
        return nullptr;
    }

    const auto numsatst = static_cast<int>((vec.size() - ((vec.size() - 4) % 4)) / 4);
#include <iostream>
    std::cout << numsatst << " " << vec.size() << std::endl;

    const auto nm = static_cast<struct gxgsv_msg*>(calloc(1, sizeof(struct gxgsv_msg)));
    if (nm == nullptr) {
        free(callv);
        return nullptr;
    }

    const auto sentences = static_cast<int8_t>(stoi_s(vec[0]));
    const auto sentnum = static_cast<int8_t>(stoi_s(vec[1]));
    const auto numsats = static_cast<int16_t>(stoi_s(vec[2]));
    const auto sigid = vec.size() >= ((numsatst * 4) + 3) ? static_cast<int16_t>(stoi_s(vec[((numsatst * 4) + 3)])) : 0;
    const auto sats = static_cast<struct iwsat**>(calloc(4, sizeof(struct iwsat*)));
    if (sats == nullptr) {
        free(callv);
        free(nm);
        return nullptr;
    }
    for (auto i = 0; i < numsatst; i++) {
        sats[i] = static_cast<struct iwsat*>(calloc(1, sizeof(struct iwsat)));
        if (sats[i] == nullptr) {
            free(callv);
            free(nm);
            for (auto j = 0; j < i; j++) {
                if (sats[j] != nullptr) {
                    if (sats[j]->call != nullptr) {
                        free(sats[j]->call);
                    }
                    free(sats[j]);
                }
            }
            free(sats);
            return nullptr;
        }
        const auto icall = strdup(callv);
        if (icall == nullptr) {
            free(callv);
            free(nm);
            for (auto j = 0; j < i; j++) {
                if (sats[j] != nullptr) {
                    if (sats[j]->call != nullptr) {
                        free(sats[j]->call);
                    }
                    free(sats[j]);
                }
            }
            free(sats[i]);
            free(sats);
            return nullptr;
        }
        const auto satid = static_cast<int16_t>(stoi_s(vec[3 + (i * 4)]));
        const auto azimuth = static_cast<int16_t>(stoi_s(vec[5 + (i * 4)]));

        const auto elev = static_cast<int8_t>(stoi_s(vec[4 + (i * 4)]));
        const auto cno = static_cast<int8_t>(stoi_s(vec[6 + (i * 4)]));

        sats[i]->call = icall;
        sats[i]->satid = satid;
        sats[i]->azimuth = azimuth;
        sats[i]->elev = elev;
        sats[i]->cno = cno;
        sats[i]->active = true;
    }
    nm->call = callv;
    nm->sentnum = sentnum;
    nm->sentences = sentences;
    nm->sigid = sigid;
    nm->sats = sats;
    nm->numsats = numsats;

    return nm;
}