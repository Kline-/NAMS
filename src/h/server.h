#ifndef DEC_SERVER_H
#define DEC_SERVER_H

#include "socket.h"

class Server : public Utils {
    public:
        Server();
        ~Server();

        // Core
        bool InitSocket( Socket* socket );
        bool LoadCommands() const;
        const void NewConnection();
        bool PollSockets();
        bool ProcessInput();
        const void Startup();
        const void Shutdown( const sint_t status );
        const void Update();

        // Query
        string gHost();
        uint_t gPort() const { return m_port; }
        uint_t gPulseRate() const { return m_pulse_rate; }
        string gTimeBoot() const;
        Socket* gSocket() const { return m_socket; }
        ITER( list, Socket*, gSocketNext() ) const { return m_socket_next; }
        bool iRunning() const { return !m_shutdown; }

        // Manipulate
        bool sPort( const uint_t port );
        bool sPulseRate( const uint_t rate );
        bool sSocket( Socket* socket );
        const void sSocketNext( ITER( list, Socket*, socket ) ) { m_socket_next = socket; return; }
        const void sTimeBoot();

    private:
        uint_t  m_bytes_recvd;
        uint_t  m_bytes_sent;
        uint_t  m_port;
        uint_t  m_pulse_rate;
        bool    m_shutdown;
        Socket* m_socket;
        ITER( list, Socket*, m_socket_next );
        time_t  m_time_boot;
};

#endif
