#ifndef DEC_ENUM_H
#define DEC_ENUM_H

// uint_t   Socket
enum {
    SOC_STATE_DISCONNECTED = 0,
    SOC_STATE_LOGIN_SCREEN = 1,
    SOC_STATE_PLAYING      = 100,
    MAX_SOC_STATE          = 101
};

// bitset<CFG_MEM_MAX_BITSET>    Utils
enum {
    UTILS_DEBUG       = 0,
    UTILS_IGNORE_CASE = 1,
    UTILS_RAW         = 2,
    UTILS_TYPE_ERROR  = 3,
    UTILS_TYPE_INFO   = 4,
    UTILS_TYPE_SOCKET = 5,
    MAX_UTILS         = 6
};

#endif
