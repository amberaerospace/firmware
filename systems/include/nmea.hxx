#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <cstdbool>

enum nmea_quality {
    NMQ_INVALID = 0,
    NMQ_NONRTKFIX = 1,
    NMQ_RTKFIX = 4,
    NMQ_FLOAT = 5,
    NMQ_DEADRCK = 6
};

struct nmea_clock {
    int8_t hours;
    int8_t minutes;
    double seconds;
};

struct nmea_lat {
    int8_t latdeg;
    long double lat;
};

struct nmea_lon {
    int16_t londeg;
    long double lon;
};

struct gngga_msg {
    struct nmea_clock* time;
    struct nmea_lat* lat;
    char latns;
    struct nmea_lon* lon;
    char lonew;
    double alt;
    char altunit;
    double hdop;
    int8_t quality;
    int8_t satcnt;
    float age;
    int16_t stid;
};

struct gngll_msg {
    struct nmea_lat* lat;
    char latns;
    struct nmea_lon* lon;
    char lonew;
    struct nmea_clock time;
    char status;
    char mode;
};

struct gngsa_msg {
    char modeop;
    char modenav;
    int8_t ids[12];
    double pdop;
    double hdop;
    double vdop;
    int8_t gnssid;
};

struct gngst_msg {
    struct nmea_clock* time;
    double rmsrange;
    double stdmajor;
    double stdminor;
    double angmajor;
    double stdlat;
    double stdlon;
    double stdalt;
};

struct gnhdt_msg {
    double heading;
    char tind;
};

struct gnrmc_msg {
    struct nmea_clock* time;
    char status;
    struct nmea_lat* lat;
    char latns;
    struct nmea_lon* lon;
    char lonew;
    double speed;
    double course;
    int8_t day;
    int8_t month;
    int8_t year;
    char mode;
    char navstat;
    char magvarew;
    double magvar;
};

struct gnvtg_msg {
    double cogtrue;
    char cogreft;
    double cogmag;
    char cogrefm;
    double sogknot;
    char sogunitn;
    double sogkph;
    char sogunitk;
    char mode;
};

struct gnzda_msg {
    struct nmea_clock* time;
    int8_t day;
    int8_t month;
    int16_t year;
    int8_t localhrs;
    int8_t localmin;
};

struct iwsat {
    char call[3];
    bool avail;
    int16_t satid;
    int8_t elev;
    int16_t azimuth;
    int8_t cno;
};

struct gxgsv_msg {
    char call[3];
    int8_t sentences;
    int8_t sentnum;
    int16_t numsats;
    struct iwsat sats[4];
    int32_t sigid;
};

std::string nmea_ident(const std::string&);
std::vector<std::string> nmea_idents(const std::string&);
bool nmea_checksum(const std::string&);
struct nmea_clock* nmea_time(const std::string&);
struct nmea_lat* nmea_clat(const std::string&);
struct nmea_lon* nmea_clon(const std::string&);
struct gngga_msg* nmea_gngga(const std::string&);
struct gngll_msg* nmea_gngll(const std::string&);
struct gngst_msg* nmea_gngst(const std::string&);
struct gnhdt_msg* nmea_gnhdt(const std::string&);