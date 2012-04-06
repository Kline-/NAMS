#ifndef DEC_SERVER_H
#define DEC_SERVER_H

#include "socket.h"

class Server {
    public:
        Server();
        ~Server();

        // Core
        bool InitSocket( Socket* socket );
        const void NewConnection() const;
        bool PollSockets() const;
        const void Shutdown( const sint_t status );
        const void Update() const;

        // Query
        uint_t gPort() const { return m_port; }
        string gTimeBoot() const;
        string gTimeCurrent() const;
        Socket* gSocket() const { return m_socket; }
        bool isRunning() const { return !m_shutdown; }

        // Manipulate
        bool sPort( const uint_t port );
        bool sSocket( Socket* socket );
        const void sTimeBoot();
        const void sTimeCurrent();

    private:
        uint_t  m_port;
        bool    m_shutdown;
        Socket* m_socket;
        time_t  m_time_boot;
        time_t  m_time_current;
};

#endif
