#include "include/nmea.hxx"
#include "include/gnss.hxx"
#include <string>
#include <vector>
#include <cstdint>
static double ltod(const int lxd, const long double lxm, const char nsew) {
    const double rv = static_cast<double>(lxd) + (static_cast<double>(lxm) / 60.0);
    return (nsew == 'S' || nsew == 'W') ? -rv : rv;
}
bool GNSS::feed(const std::string& msg) {
    if (msg.empty()) return false;
    if (!nmea_checksum(msg)) return false;
    const std::string ident = nmea_ident(msg);
    if (ident.empty()) return false;
    if (ident == "GNTXT") return false;
    if (ident == "GNZDA") {
        // date and time
        const auto mdata = nmea_gnzda(msg);
        if (mdata == nullptr) return false;
        day = static_cast<uint8_t>(mdata->day);
        month = static_cast<uint8_t>(mdata->month);
        zdayear = mdata->year; // we get yyyy here
        if (zdayear > othyear) year = zdayear;
        hour = static_cast<uint8_t>(mdata->time->hours);
        min = static_cast<uint8_t>(mdata->time->minutes);
        sec = mdata->time->seconds;
        free(mdata->time);
        free(mdata);
        return true;
    }
    if (ident == "GNGSV") {
        // satellites in view (sats)
        const auto mdata = nmea_gxgsv(msg);
        if (mdata == nullptr) return false;
        sats = static_cast<uint8_t>(mdata->numsats);
        free(mdata->call);
        for (int i = 0; i < 4; i++) if (mdata->sats[i]) free(mdata->sats[i]);
        free(mdata);
        return true;
    }
    if (ident == "GNRMC") {
        // the minimal information
        const auto mdata = nmea_gnrmc(msg);
        if (mdata == nullptr) return false;
        hour = static_cast<uint8_t>(mdata->time->hours);
        min = static_cast<uint8_t>(mdata->time->minutes);
        sec = mdata->time->seconds;
        free(mdata->time);
        day = static_cast<uint8_t>(mdata->day);
        month = static_cast<uint8_t>(mdata->month);
        othyear = static_cast<int>(static_cast<uint8_t>(mdata->year));
        if (othyear < 80) othyear += 2000; else othyear += 1900;
        if (zdayear > othyear) year = zdayear;
        lat = ltod(mdata->lat->latdeg, mdata->lat->lat, mdata->latns);
        lon = ltod(mdata->lon->londeg, mdata->lon->lon, mdata->lonew);
        free(mdata->lon);
        free(mdata->lat);
        speed = mdata->speed * 0.514444f;
        course = mdata->course;
        if (mdata->status == 'A') {
            anyfix = true;
        }
        free(mdata);
        return true;
    }
    if (ident == "GNGSA") {
        // hpv-dop
        const auto mdata = nmea_gngsa(msg);
        if (mdata == nullptr) return false;
        hdop = mdata->hdop;
        pdop = mdata->pdop;
        vdop = mdata->vdop;
        free(mdata);
        return true;
    }
    // todo: add gga, add ids to class
    return false;
}
