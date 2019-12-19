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

#include "TcpClient.h"

using namespace std;

/** \brief Constructor */
TcpClient::TcpClient()
: m_socket(INVALID_SOCKET)
{}

/** \brief Constructor */
TcpClient::TcpClient(SOCKET socket)
: m_socket(socket)
{}

/** \brief Destructor */
TcpClient::~TcpClient()
{
    close();
}

/** \brief Connect to a TCP server */
bool TcpClient::connect(const std::string& server, const uint16_t port, const uint32_t timeout)
{
    bool ret = false;

    // Check if the socket is already opened
    if (m_socket == INVALID_SOCKET)
    {
        // Create the socket
        m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_socket != INVALID_SOCKET)
        {
            // Configure timeout
            DWORD opt_timeout = timeout;
            setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&opt_timeout), sizeof(opt_timeout));

            // Connect to the server
            struct sockaddr_in sock_addr = { 0 };
            sock_addr.sin_family = AF_INET;
            sock_addr.sin_addr.S_un.S_addr = inet_addr(server.c_str());
            sock_addr.sin_port = htons(port);
            int res = ::connect(m_socket, reinterpret_cast<const sockaddr*>(&sock_addr), sizeof(sock_addr));
            if (res == SOCKET_ERROR)
            {
                close();
            }
            else
            {
                ret = true;
            }
        }
    }

    return ret;
}

/** \brief Close the connection with the TCP server */
bool TcpClient::close()
{
    bool ret = false;

    if (m_socket != INVALID_SOCKET)
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        ret = true;
    }

    return ret;
}

/** \brief Indicates if the socket is in connected state */
bool TcpClient::isConnected()
{
    bool ret = false;

    if (m_socket != INVALID_SOCKET)
    {
        fd_set fds = { 0 };
        FD_SET(m_socket, &fds);
        timeval tv_timeout = { 0 };

        int res = select(static_cast<int>(m_socket), &fds, nullptr, nullptr, &tv_timeout);
        if (res == 0)
        {
            ret = true;
        }
        else if (res == SOCKET_ERROR)
        {
            ret = false;
        }
        else
        {
            char dummy = 0;
            res = ::recv(m_socket, &dummy, sizeof(dummy), MSG_PEEK);
            ret = (res > 0);
        }
    }

    return ret;
}

/** \brief Send data to the TCP server */
bool TcpClient::send(const void* data, const size_t size)
{
    bool ret = false;

    if (m_socket != INVALID_SOCKET)
    {
        int res = ::send(m_socket, reinterpret_cast<const char*>(data), static_cast<int>(size), 0);
        ret = (res == static_cast<int>(size));
    }

    return ret;
}

/** \brief Receive data from the TCP server */
bool TcpClient::recv(void* data, const size_t size, size_t& received, const uint32_t timeout)
{
    bool ret = false;

    if (m_socket != INVALID_SOCKET)
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
            tv_timeout.tv_sec = timeout / 1000u;
            tv_timeout.tv_usec = (timeout - (tv_timeout.tv_sec * 1000u)) * 1000u;

            int res = select(static_cast<int>(m_socket), &fds, nullptr, nullptr, &tv_timeout);
            ret = ((res != 0) && (res != SOCKET_ERROR));
            if (ret)
            {
                res = ::recv(m_socket, reinterpret_cast<char*>(data), static_cast<int>(size), 0);
                if ((res == SOCKET_ERROR) || (res == 0))
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

/** \brief Wait for multiple clients to be ready for a read operation */
bool TcpClient::waitReadReady(const std::list<TcpClient*>& clients, std::vector<TcpClient*>& ready_clients, const uint32_t timeout)
{
    bool ret = false;

    if ((clients.size() > 0) && (clients.size() <= FD_SETSIZE))
    {
        fd_set fds = { 0 };
        for (auto iter = clients.begin(); iter != clients.end(); ++iter)
        {
            TcpClient* client = *iter;
            FD_SET(client->m_socket, &fds);
        }
        timeval tv_timeout;
        tv_timeout.tv_sec = timeout / 1000u;
        tv_timeout.tv_usec = (timeout - (tv_timeout.tv_sec * 1000u)) * 1000u;
        
        int res = select(static_cast<int>(clients.size()), &fds, nullptr, nullptr, &tv_timeout);
        ret = ((res != 0) && (res != SOCKET_ERROR));
        if (ret)
        {
            for (auto iter = clients.begin(); iter != clients.end(); ++iter)
            {
                TcpClient* client = *iter;
                if (FD_ISSET(client->m_socket, &fds))
                {
                    ready_clients.push_back(client);
                }
            }
        }
    }

    return ret;
}