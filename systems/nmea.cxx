#include "include/nmea.hxx"
#include <cstring>
#include <cstdlib>

char* nmea_ident(const char* message) {
    // we expect message to be already splitted and stripped from '\r\n'
    if (message == NULL) return NULL;
    if (*message == '$') {
        message++;
        if (strlen(message) >= 5) {
            char* buf = (char*)calloc(6, sizeof(char));
            if (buf == NULL) return NULL;
            strncpy(buf, message, 5);
            if (strcmp(buf, "GNGGA") == 0)      { free(buf); return "GNGGA"; }
            if (strcmp(buf, "GNGLL") == 0)      { free(buf); return "GNGGL"; }
            if (strcmp(buf, "GNGSA") == 0)      { free(buf); return "GNGSA"; }
            if (strcmp(buf, "GNGST") == 0)      { free(buf); return "GNGST"; }
            if (strcmp(buf, "GNHDT") == 0)      { free(buf); return "GNHDT"; }
            if (strcmp(buf, "GNRMC") == 0)      { free(buf); return "GNRMC"; }
            if (strcmp(buf, "GNVTG") == 0)      { free(buf); return "GNVTG"; }
            if (strcmp(buf, "GNZDA") == 0)      { free(buf); return "GNZDA"; }
            if (strcmp(buf + 2, "GSV") == 0)    { free(buf); return "GNGSV"; }
            free(buf);
            return NULL;  
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
    return NULL;
}