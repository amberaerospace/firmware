#pragma once
#ifndef _CHECKS_HXX_
#define _CHECKS_HXX_

enum pfchecks {
    PREFLIGHT_NO_GPS, // "no GPS found"
    PREFLIGHT_NO_MAG, // "no magnetometer found"
    PREFLIGHT_NO_BAR, // "no barometer found"
    PREFLIGHT_NO_IMU, // "no inertial measurement unit found"
    PREFLIGHT_NO_ACC, // "no accelerometer found"
    PREFLIGHT_NO_TOF, // "no ToF sensor found"
    PREFLIGHT_NO_CLP, // "no area safety"
    PREFLIGHT_NO_ACT, // "no actuators available"
    PREFLIGHT_NO_BAT, // "main aircraft power supply is not yet enabled"
    PREFLIGHT_NO_CNT, // "aircraft has no control sources"
    PREFLIGHT_NO_ASP, // "no airspeed sensor"
    PREFLIGHT_NO_GSP, // "no ground speed sensor"
    PREFLIGHT_NO_CON, // "no flight console"
    PREFLIGHT_INF_BAT, // "main aircraft power supply is insufficient for flight"
    PREFLIGHT_CHK_CAL, // "check aircraft sensor calibration"
    PREFLIGHT_GPS_DFT, // "GPS coordinate drift"
    PREFLIGHT_CL_PROP, // "clear prop before flight"
    PREFLIGHT_ACT_FAIL, // "actuators failure"
    PREFLIGHT_WTH_WIND, // "extreme windy conditions"
};

#endif