#ifndef DEC_UTILS_H
#define DEC_UTILS_H

class Utils {
    public:
        Utils();
        ~Utils();

        // Core
        class DeleteObject
        {
            public:
                template <class T> inline void operator() (const T* ptr) const { delete ptr; };
        };
        template <class T> inline string toLower( const T& t )  { stringstream ss; ss << nouppercase << t; return ss.str(); }
        template <class T> inline string toString( const T& t ) { stringstream ss; ss << t; return ss.str(); }
        template <class T> inline string toUpper( const T& t )  { stringstream ss; ss << uppercase << t; return ss.str(); }

        string _FormatString( const uint_t narg, const bitset<CFG_MEM_MAX_BITSET> flags, const string caller, const string fmt, ... );
        string __FormatString( const uint_t narg, const bitset<CFG_MEM_MAX_BITSET> flags, const string caller, const string fmt, va_list val );
        #define FormatString( flags, fmt, ... ) _FormatString( PP_NARG( __VA_ARGS__ ), flags, _caller_, fmt, ##__VA_ARGS__ )
        void _Logger( const uint_t narg, const bitset<CFG_MEM_MAX_BITSET> flags, const string caller, const string fmt, ... );
        #define Logger( flags, fmt, ... ) _Logger( PP_NARG( __VA_ARGS__ ), flags, _caller_, fmt, ##__VA_ARGS__ )
        bool iNumber( const string input );
        uint_t NumChar( const string input, const string item );
        vector<string> StrNewlines( const string input );
        vector<string> StrTokens( const string input );

        // Query
        string gTimeCurrent() const;

        // Manipulate
        const void sTimeCurrent();

    private:
        time_t  m_time_current;
};

#endif
