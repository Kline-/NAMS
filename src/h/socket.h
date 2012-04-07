#ifndef DEC_SOCKET_H
#define DEC_SOCKET_H

#include "lists.h"
#include "enum.h"
#include "server.h"

class Socket : public Utils {
    public:
        Socket();
        ~Socket();

    // Core
    bool Bind( const uint_t port, const string addr );
    const void Disconnect( const string msg = "" );
    bool Listen();
    bool PendingCommand() const { return !m_command_queue.empty(); }
    bool PendingOutput() const { return !m_output.empty(); }
    bool ProcessCommand();
    bool ProcessInput();
    bool QueueCommand( const string command );
    bool Recv();
    const void ResolveHostname();
    const void Send( const string msg );
    bool Send();

    // Query
    sint_t gDescriptor() const { return m_descriptor; }
    string gHost() const { return m_host; }
    uint_t gIdle() const { return m_idle; }
    uint_t gPort() const { return m_port; }
    Server* gServer() const { return m_server; }
    uint_t gState() const { return m_state; }
    bool iValid() const { return m_descriptor > 0; }

    // Manipulate
    bool sDescriptor( const sint_t descriptor );
    bool sHost( const string host );
    bool sIdle( const uint_t idle );
    bool sPort( const uint_t port );
    static void* tResolveHostname( void* data );
    bool sServer( Server* server );
    bool sState( const uint_t state );

    private:
        uint_t  m_bytes_recvd;
        uint_t  m_bytes_sent;
        vector<string> m_command_queue;
        sint_t  m_descriptor;
        string  m_host;
        uint_t  m_idle;
        string  m_input;
        string  m_output;
        uint_t  m_port;
        Server* m_server;
        uint_t  m_state;
};

#endif
