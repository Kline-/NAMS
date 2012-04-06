#ifndef DEC_SERVER_H
#define DEC_SERVER_H

#include "socket.h"

class ServerInfo {
    public:
        ServerInfo();
        ~ServerInfo();

        const void Shutdown( const sint_t status );
        const void Update() const;

        uint_t gPort() const { return m_port; }
        bool sPort( const uint_t port );
        bool isRunning() const { return m_shutdown; }
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
