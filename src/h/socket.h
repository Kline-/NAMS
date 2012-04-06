#ifndef DEC_SOCKET_H
#define DEC_SOCKET_H

#include "lists.h"

class Socket {
    public:
        Socket();
        ~Socket();

    // Core

    // Query
    bool Accept( Socket* socket ) const;
    sint_t gDescriptor() const { return m_descriptor; }
    bool isValid() const { return m_descriptor > 0; }
    bool Listen() const;

    // Manipulate
    bool Bind( const uint_t port, const string addr = "" );

    private:
        sint_t m_descriptor;
};

#endif
