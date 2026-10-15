#include "include/nmea.hxx"
#include "include/gnss.hxx"
#include <string>
#include <vector>
#include <cstdint>

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
    return false;
}
