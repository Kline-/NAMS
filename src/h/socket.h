#ifndef DEC_SOCKET_H
#define DEC_SOCKET_H

#include "lists.h"

class Socket {
    public:
        Socket();
        ~Socket();

    bool Bind( const uint_t port, const string addr = "" );
    bool isValid() const { return m_descriptor > 0; }
    bool Listen() const;

    private:
        sint_t m_descriptor;
};

#endif
