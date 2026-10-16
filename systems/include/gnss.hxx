#pragma once
#include "nmea.hxx"
#include <string>
class GNSS {
    int zdayear = 0;
    int othyear = 0;
    double lat = 0;
    double lon = 0;
    int day = 0;
    int month = 0;
    int year = 0;
    int hour = 0;
    int min = 0;
    double sec = 0;
    int sats = 0;
    double hdop = 0;
    double pdop = 0;
    double vdop = 0;
    double speed = 0;
    double course = 0;
    int16_t gsaids[12] = {0};
    bool anyfix = false;
public:
    bool feed(const std::string&);
};