/*
Copyright(c) 2019 Cedric Jimenez

This file is part of cantools.

cantools is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

cantools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with cantools.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "TcpClient.h"
#include "NoCopy.h"

#include <string>
#include <functional>
#include <thread>

#include <Windows.h>

/** \brief TCP server */
class TcpServer : public INoCopy
{
    public:

        /** \brief Callback for new TCP client connection */
        typedef std::function<void(TcpClient*)> new_client_callback;


        /** \brief Constructor */
        TcpServer();
        /** \brief Destructor */
        virtual ~TcpServer();


        /** \brief Start the TCP server */
        bool start(const std::string& listen_address, const uint16_t listen_port, const new_client_callback client_callback);

        /** \brief Stop the TCP server */
        bool stop();
     
    private:

        /** \brief TCP socket */
        SOCKET m_socket;

        /** \brief Server's thread */
        std::thread* m_thread;

        /** \brief Callback for new TCP client connection */
        new_client_callback m_client_callback;


        /** \brief Server's thread procedure */
        void listenThread();
};


#endif // TCPSERVER_H
