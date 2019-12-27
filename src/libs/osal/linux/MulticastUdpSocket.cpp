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

#include "MulticastUdpSocket.h"

#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <limits>
using namespace std;

/** \brief Constructor */
MulticastUdpSocket::MulticastUdpSocket()
: m_socket(-1)
, m_mcast_addr()
{}

/** \brief Destructor */
MulticastUdpSocket::~MulticastUdpSocket()
{
    close();
}

/** \brief Open the socket */
bool MulticastUdpSocket::open(const std::string& ip_address, const uint16_t port)
{
    bool ret = false;

    // Check if the socket is already opened
    if (m_socket < 0)
    {
        // Create the socket
        m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (m_socket < 0)
        {
            // Allow reuse of IP address
            int reuse = 1;
            int res = setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reuse), static_cast<int>(sizeof(reuse)));
            if (res == 0)
            {
                // Bind socket to multicast address
                memset(&m_mcast_addr, 0, sizeof(m_mcast_addr));
                m_mcast_addr.sin_family = AF_INET;
                m_mcast_addr.sin_addr.s_addr = htonl(INADDR_ANY);
                m_mcast_addr.sin_port = htons(port);
                res = ::bind(m_socket, reinterpret_cast<sockaddr*>(&m_mcast_addr), sizeof(m_mcast_addr));
                if (res == 0)
                {
                    // Subscribe to multicast group
                    ip_mreq mreq = { 0 };
                    inet_pton(AF_INET, ip_address.c_str(), &mreq.imr_multiaddr.s_addr);
                    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
                    res = setsockopt(m_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<char*>(&mreq), static_cast<int>(sizeof(mreq)));
                    if (res == 0)
                    {
                        m_mcast_addr.sin_addr = mreq.imr_multiaddr;
                        ret = true;
                    }
                }
            }
        }
        if (!ret)
        {
            close();
        }
    }

    return ret;
}

/** \brief Close the socket */
bool MulticastUdpSocket::close()
{
    bool ret = false;

    if (m_socket >= 0)
    {
        ::close(m_socket);
        m_socket = -1;
    }

    return ret;
}

/** \brief Send data through the socket */
bool MulticastUdpSocket::send(const void* data, const size_t size)
{
    bool ret = false;

    if (m_socket >= 0)
    {
        int res = sendto(m_socket, reinterpret_cast<const char*>(data), static_cast<int>(size), 0, reinterpret_cast<sockaddr*>(&m_mcast_addr), static_cast<int>(sizeof(m_mcast_addr)));
        ret = (res == static_cast<int>(size));
    }

    return ret;
}

/** \brief Receive data from the socket */
bool MulticastUdpSocket::recv(void* data, const size_t size, size_t& received)
{
    return recv(data, size, received, numeric_limits<uint32_t>::max());
}

/** \brief Receive data from the socket */
bool MulticastUdpSocket::recv(void* data, const size_t size, size_t& received, const uint32_t timeout)
{
    bool ret = false;

    if (m_socket >= 0)
    {
        if (size == 0)
        {
            received = 0;
            ret = true;
        }
        else
        {
            fd_set fds = { 0 };
            FD_SET(m_socket, &fds);
            timeval tv_timeout;
            timeval* ptimeout = nullptr;            
            if (timeout != numeric_limits<uint32_t>::max())
            {
                ptimeout = &tv_timeout;
                tv_timeout.tv_sec = timeout / 1000u;
                tv_timeout.tv_usec = (timeout - (tv_timeout.tv_sec * 1000u)) * 1000u;
            }

            int res = select(static_cast<int>(m_socket), &fds, nullptr, nullptr, ptimeout);
            ret = (res > 0);
            if (ret)
            {
                sockaddr_in addr = { 0 };
                socklen_t addr_len = sizeof(addr);
                res = recvfrom(m_socket, reinterpret_cast<char*>(data), static_cast<int>(size), 0, reinterpret_cast<sockaddr*>(&addr), &addr_len);
                if (res <= 0)
                {
                    ret = false;
                }
                else
                {
                    received = static_cast<size_t>(res);
                }
            }
        }
    }

    return ret;
}
