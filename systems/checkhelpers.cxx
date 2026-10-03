#include "include/checkhelpers.hxx"
#include <cstring>

/*
    Safety and systems checking
*/

char* strpreflight(enum preflight_checks pf) {
    switch (pf) {
        case PREFLIGHT_NO_GPS:
            return strdup("No GPS found on-board.");
        case PREFLIGHT_NO_MAG:
            return strdup("No magnetometer found on-board.");
        case PREFLIGHT_NO_BAR:
            return strdup("No barometer found on-board.");
        case PREFLIGHT_NO_IMU:
            return strdup("No inertial measurement unit found on-board.");
        case PREFLIGHT_NO_ACC:
            return strdup("No accelerometer found on-board.");
        case PREFLIGHT_NO_TOF:
            return strdup("No time of flight sensor found on-board.");
        case PREFLIGHT_NO_CLP:
            return strdup("Area safety sensor not found on-board.");
        case PREFLIGHT_NO_ACT:
            return strdup("Actuators are not installed or not ready.");
        case PREFLIGHT_NO_BAT:
            return strdup("Main aircraft power supply is not yet enabled.");
        case PREFLIGHT_NO_CNT:
            return strdup("Aircraft has no control source.");
        case PREFLIGHT_NO_ASP:
            return strdup("Airspeed sensor not found on-board.");
        case PREFLIGHT_NO_GSP:
            return strdup("Ground speed sensor not found on-board.");
        case PREFLIGHT_NO_CON:
            return strdup("No flight console connected.");
        case PREFLIGHT_INF_BAT:
            return strdup("Main aircraft power supply is insufficient for flight.");
        case PREFLIGHT_CHK_CAL:
            return strdup("Check sensor calibration.");
        case PREFLIGHT_GPS_DFT:
            return strdup("GPS coordinate drift detected.");
        case PREFLIGHT_CL_PROP:
            return strdup("Clear prop before flight.");
        case PREFLIGHT_ACT_FAIL:
            return strdup("Actuator system failure.");
        case PREFLIGHT_WTH_WIND:
            return strdup("Extreme weather conditions.");
        case PREFLIGHT_SYS_FAIL:
            return strdup("Aircraft system failure.");
        default:
            return NULL;
    }
}