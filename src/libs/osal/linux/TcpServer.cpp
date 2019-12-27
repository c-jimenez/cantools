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

#include "TcpServer.h"

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
using namespace std;

/** \brief Constructor */
TcpServer::TcpServer()
: m_socket(-1)
, m_thread(nullptr)
{}

/** \brief Destructor */
TcpServer::~TcpServer()
{
    stop();
}

/** \brief Start the TCP server */
bool TcpServer::start(const std::string& listen_address, const uint16_t listen_port, const new_client_callback client_callback)
{
    bool ret = false;

    // Check if the socket is already opened
    if (m_socket < 0)
    {
        // Create the socket
        m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_socket >= 0)
        {
            // Bind socket to listen address
            struct sockaddr_in sock_addr = { 0 };
            sock_addr.sin_family = AF_INET;
            if (listen_address.empty())
            {
                sock_addr.sin_addr.s_addr = INADDR_ANY;
            }
            else
            {
                sock_addr.sin_addr.s_addr = inet_addr(listen_address.c_str());
            }
            sock_addr.sin_port = htons(listen_port);
            int res = ::bind(m_socket, reinterpret_cast<sockaddr*>(&sock_addr), sizeof(sock_addr));
            if (res == 0)
            {
                // Start listening for incoming connections
                res = listen(m_socket, SOMAXCONN);
            }
            if (res < 0)
            {
                stop();
            }
            else
            {
                // Create server thread
                m_client_callback = client_callback;
                m_thread = new thread(&TcpServer::listenThread, this);
                ret = true;
            }
        }
    }

    return ret;
}

/** \brief Stop the TCP server */
bool TcpServer::stop()
{
    bool ret = false;

    if (m_socket >= 0)
    {
        close(m_socket);
        m_socket = -1;
        if (m_thread != nullptr)
        {
            m_thread->join();
            delete m_thread;
            m_thread = nullptr;
        }
    }

    return ret;
}

/** \brief Server's thread procedure */
void TcpServer::listenThread()
{
    // Wait for incoming connection
    int client_socket;
    while ((client_socket = accept(m_socket, nullptr, nullptr)) >= 0)
    {
        // Allocate new TCP client
        TcpClient* client = new TcpClient(client_socket);

        // Notify connection
        m_client_callback(client);
    }
}
