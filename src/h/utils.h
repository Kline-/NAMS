#ifndef DEC_UTILS_H
#define DEC_UTILS_H

#include "server.h"

namespace Utils {
    class DeleteObject
    {
        public:
            template <class T> inline void operator() (const T* ptr) const { delete ptr; };
    };
    string _FormatString( const uint_t narg, const bitset<CFG_MEM_MAX_BITSET> flags, const string caller, const string fmt, ... );
    string __FormatString( const uint_t narg, const bitset<CFG_MEM_MAX_BITSET> flags, const string caller, const string fmt, va_list val );
    #define FormatString( flags, fmt, ... ) _FormatString( PP_NARG( __VA_ARGS__ ), flags, _caller_, fmt, ##__VA_ARGS__ )
    bool isNumber( const string input );
    void _Logger( const uint_t narg, const bitset<CFG_MEM_MAX_BITSET> flags, const string caller, const string fmt, ... );
    #define Logger( flags, fmt, ... ) _Logger( PP_NARG( __VA_ARGS__ ), flags, _caller_, fmt, ##__VA_ARGS__ )
    uint_t NumChar( const string input, const string item );
    vector<string> StrTokens( const string input );
    template <class T> inline string toLower( const T& t )  { stringstream ss; ss << nouppercase << t; return ss.str(); }
    template <class T> inline string toString( const T& t ) { stringstream ss; ss << t; return ss.str(); }
    template <class T> inline string toUpper( const T& t )  { stringstream ss; ss << uppercase << t; return ss.str(); }
};

// Thanks to Laurent Deniau @ https://groups.google.com/d/msg/comp.std.c/d-6Mj5Lko_s/5R6bMWTEbzQJ
#define PP_NARG(...) \
         PP_NARG_(__VA_ARGS__,PP_RSEQ_N())
#define PP_NARG_(...) \
         PP_ARG_N(__VA_ARGS__)
#define PP_ARG_N( \
         _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
         _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
         _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
         _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
         _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
         _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
         _61,_62,_63,N,...) N
#define PP_RSEQ_N() \
         63,62,61,60,                   \
         59,58,57,56,55,54,53,52,51,50, \
         49,48,47,46,45,44,43,42,41,40, \
         39,38,37,36,35,34,33,32,31,30, \
         29,28,27,26,25,24,23,22,21,20, \
         19,18,17,16,15,14,13,12,11,10, \
         9,8,7,6,5,4,3,2,1,0

#define STR(x) #x
#define SX(x) STR(x)
#define _caller_ __FILE__ ":" SX(__LINE__)

#define CSTR( func ) ( func ).c_str()
#define ITER( type, name ) list<type>::iterator name

#endif
