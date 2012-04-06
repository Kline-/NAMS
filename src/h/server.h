#ifndef DEC_SERVER_H
#define DEC_SERVER_H

#include "socket.h"

class Server {
    public:
        Server();
        ~Server();

        // Core
        bool PollSockets() const;
        const void Shutdown( const sint_t status );
        const void Update() const;

        // Query
        bool isRunning() const { return !m_shutdown; }

        // Manipulate
        uint_t gPort() const { return m_port; }
        bool sPort( const uint_t port );

        bool sSocket( Socket* socket );

        string gTimeBoot() const;
        const void sTimeBoot();

        string gTimeCurrent() const;
        const void sTimeCurrent();

    private:
        uint_t  m_port;
        bool    m_shutdown;
        Socket* m_socket;
        time_t  m_time_boot;
        time_t  m_time_current;
};

#endif
