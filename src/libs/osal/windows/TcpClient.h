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

#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "NoCopy.h"

#include <string>
#include <vector>
#include <list>

#include <Windows.h>

/** \brief TCP client */
class TcpClient : public INoCopy
{
    public:

        /** \brief Constructor */
        TcpClient();
        /** \brief Constructor */
        TcpClient(SOCKET socket);
        /** \brief Destructor */
        virtual ~TcpClient();


        /** \brief Connect to a TCP server */
        bool connect(const std::string& server, const uint16_t port, const uint32_t timeout);

        /** \brief Close the connection with the TCP server */
        bool close();

        /** \brief Indicates if the socket is in connected state */
        bool isConnected();

        /** \brief Send data to the TCP server */
        bool send(const void* data, const size_t size);

        /** \brief Receive data from the TCP server */
        bool recv(void* data, const size_t size, size_t& received, const uint32_t timeout);


        /** \brief Wait for multiple clients to be ready for a read operation */
        static bool waitReadReady(const std::list<TcpClient*>& clients, std::vector<TcpClient*>& ready_clients, const uint32_t timeout);
     
    private:

        /** \brief TCP socket */
        SOCKET m_socket;
};


#endif // TCPCLIENT_H
