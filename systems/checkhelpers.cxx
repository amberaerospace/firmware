#include "include/checkhelpers.hxx"
#include <string>

/*
 * Safety and check helper functions
 */

std::string strpreflight(const enum preflight_checks pf) {
    switch (pf) {
        case PREFLIGHT_NO_GPS:
            return "No GPS found on-board.";
        case PREFLIGHT_NO_MAG:
            return "No magnetometer found on-board.";
        case PREFLIGHT_NO_BAR:
            return "No barometer found on-board.";
        case PREFLIGHT_NO_IMU:
            return "No inertial measurement unit found on-board.";
        case PREFLIGHT_NO_ACC:
            return "No accelerometer found on-board.";
        case PREFLIGHT_NO_TOF:
            return "No time of flight sensor found on-board.";
        case PREFLIGHT_NO_CLP:
            return "Area safety sensor not found on-board.";
        case PREFLIGHT_NO_ACT:
            return "Actuators are not installed or not ready.";
        case PREFLIGHT_NO_BAT:
            return "Main aircraft power supply is not yet enabled.";
        case PREFLIGHT_NO_CNT:
            return "Aircraft has no control source.";
        case PREFLIGHT_NO_ASP:
            return "Airspeed sensor not found on-board.";
        case PREFLIGHT_NO_GSP:
            return "Ground speed sensor not found on-board.";
        case PREFLIGHT_NO_CON:
            return "No flight console connected.";
        case PREFLIGHT_INF_BAT:
            return "Main aircraft power supply is insufficient for flight.";
        case PREFLIGHT_CHK_CAL:
            return "Check sensor calibration.";
        case PREFLIGHT_GPS_DFT:
            return "GPS coordinate drift detected.";
        case PREFLIGHT_CL_PROP:
            return "Clear prop before flight.";
        case PREFLIGHT_ACT_FAIL:
            return "Actuator system failure.";
        case PREFLIGHT_WTH_WIND:
            return "Extreme weather conditions.";
        case PREFLIGHT_SYS_FAIL:
            return "Aircraft system failure.";
        default:
            return "";
    }
}