#pragma once
#ifndef _NMEA_HXX_
#define _NMEA_HXX_
#include <cstdint>

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
    int8_t seconds;
    int16_t millis;
};

struct gngga_msg {
    struct nmea_clock time;
    int8_t latdeg;
    long double lat;
    char latns;
    int16_t londeg;
    long double lon;
    char lonew;
    double alt;
    char altunit;
    double hdop;
    enum nmea_quality quality;
    int8_t satcnt;
    float age;
    int16_t stid;
};

struct gngll_msg {
    int8_t latdeg;
    long double lat;
    char latns;
    int16_t londeg;
    long double lon;
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
    struct nmea_clock time;
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
    struct nmea_clock time;
    char status;
    int8_t latdeg;
    long double lat;
    char latns;
    int16_t londeg;
    long double lon;
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
    struct nmea_clock time;
    int8_t day;
    int8_t month;
    int16_t year;
    int8_t localhrs;
    int8_t localmin;
};

struct iwsat {
    char call[3];
    int8_t avail; // 0 for not available, 1 for available
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

#endif