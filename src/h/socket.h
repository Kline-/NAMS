#ifndef DEC_SOCKET_H
#define DEC_SOCKET_H

#include "lists.h"

class Socket {
    public:
        Socket();
        ~Socket();

    // Core
    const void Output( const string msg );
    bool Send();

    // Query
    sint_t gDescriptor() const { return m_descriptor; }
    string gHost() const { return m_host; }
    bool Listen() const;
    uint_t gPort() const { return m_port; }
    bool isValid() const { return m_descriptor > 0; }

    // Manipulate
    bool Bind( const uint_t port, const string addr = "" );
    bool sDescriptor( const sint_t descriptor );
    bool sHost( const string host );
    bool sPort( const uint_t port );
    const void ResolveHostname();
    static void* tResolveHostname( void* data );

    private:
        sint_t m_descriptor;
        string m_host;
        string m_output;
        uint_t m_port;
};

#endif
