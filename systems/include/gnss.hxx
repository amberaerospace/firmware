#pragma once
#include "nmea.hxx"
#include <string>
class GNSS {
    int zdayear = 0;
    int othyear = 0;
public:
    double lat = 0;
    double lon = 0;
    int day = 0;
    int month = 0;
    int year = 0;
    int hour = 0;
    int min = 0;
    double sec = 0;
    int sats = 0;
    bool feed(const std::string&);
};