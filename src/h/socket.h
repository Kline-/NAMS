#ifndef DEC_SOCKET_H
#define DEC_SOCKET_H

#include "lists.h"
#include "enum.h"

class Socket {
    public:
        Socket();
        ~Socket();

    // Core
    bool Bind( const uint_t port, const string addr );
    const void Disconnect( const string msg = "" );
    bool Listen() const;
    bool PendingOutput() const { return !m_output.empty(); }
    bool Recv();
    const void ResolveHostname();
    const void Send( const string msg );
    bool Send();

    // Query
    sint_t gDescriptor() const { return m_descriptor; }
    string gHost() const { return m_host; }
    uint_t gIdle() const { return m_idle; }
    uint_t gPort() const { return m_port; }
    uint_t gState() const { return m_state; }
    bool isValid() const { return m_descriptor > 0; }

    // Manipulate
    bool sDescriptor( const sint_t descriptor );
    bool sHost( const string host );
    bool sIdle( const uint_t idle );
    bool sPort( const uint_t port );
    static void* tResolveHostname( void* data );
    bool sState( const uint_t state );

    private:
        sint_t m_descriptor;
        string m_host;
        uint_t m_idle;
        string m_input;
        string m_output;
        uint_t m_port;
        uint_t m_state;
};

#endif
