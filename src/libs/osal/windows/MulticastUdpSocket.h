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

#ifndef MULTICASTUDPSOCKET_H
#define MULTICASTUDPSOCKET_H

#include "NoCopy.h"

#include <string>

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX
#include <ws2tcpip.h>
#include <Windows.h>
#undef NOMINMAX


/** \brief Multicast UDP socket */
class MulticastUdpSocket : public INoCopy
{
    public:

        /** \brief Constructor */
        MulticastUdpSocket();
        /** \brief Destructor */
        virtual ~MulticastUdpSocket();


        /** \brief Open the socket */
        bool open(const std::string& ip_address, const uint16_t port);

        /** \brief Close the socket */
        bool close();

        /** \brief Send data through the socket */
        bool send(const void* data, const size_t size);

        /** \brief Receive data from the socket */
        bool recv(void* data, const size_t size, size_t& received);

        /** \brief Receive data from the socket */
        bool recv(void* data, const size_t size, size_t& received, const uint32_t timeout);


    private:

        /** \brief UDP socket */
        SOCKET m_socket;

        /** \brief UDP multicast group address */
        struct sockaddr_in m_mcast_addr;
};


#endif // MULTICASTUDPSOCKET_H
