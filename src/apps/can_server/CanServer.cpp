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

#include "CanServer.h"
#include "CanClient.h"

#include <iostream>
#include <chrono>
using namespace std;

/** \brief Constructor */
CanServer::CanServer()
: m_tcp_server()
, m_clients()
, m_disconnected_clients()
, m_thread(nullptr)
, m_mutex()
, m_timeout(0)
{}

/** \brief Destructor */
CanServer::~CanServer()
{}

/** \brief Start the server */
bool CanServer::start(const std::string& ip_address, const uint16_t port, const uint32_t timeout)
{
    // Start TCP server
    m_timeout = timeout;
    auto new_client_callback = std::bind(&CanServer::onNewClient, this, placeholders::_1);
    bool ret = m_tcp_server.start(ip_address, port, new_client_callback);
    if (ret)
    {
        // Start communication thread
        m_thread = new thread(&CanServer::cleanupThread, this);
    }

    return ret;
}

/** \brief Called when a new client is connected to the server */
void CanServer::onNewClient(TcpClient* tcp_client)
{
    // Add the client to the list
    CanClient* can_client = new CanClient(tcp_client, m_timeout);
    {
        const lock_guard<mutex> guard(m_mutex);
        m_clients.push_back(can_client);
    }

    cout << "New client (id = " << can_client->id() << ") connected!" << endl;
    auto disconnect_callback = bind(&CanServer::onLostClient, this, placeholders::_1);
    auto msg_received_callback = bind(&CanServer::onCanMsgReceived, this, placeholders::_1, placeholders::_2);
    can_client->start(disconnect_callback, msg_received_callback);
}

/** \brief Called when a client is disconnected from the server */
void CanServer::onLostClient(CanClient& can_client)
{
    // Remove client from the list
    {
        const lock_guard<mutex> guard(m_mutex);
        m_clients.remove(&can_client);
        m_disconnected_clients.push_back(&can_client);
    }

    cout << "Client (id = " << can_client.id() << ") disconnected!" << endl;
}

/** \brief Called when a CAN message has been received by a client */
void CanServer::onCanMsgReceived(CanClient& can_client, const CanMsg& can_msg)
{
    // Dispatch message to all other clients
    const lock_guard<mutex> guard(m_mutex);
    for (auto iter = m_clients.begin(); iter != m_clients.end(); ++iter)
    {
        CanClient* client = *iter;
        if (client != &can_client)
        {
            client->sendMsg(can_msg);
        }
    }
}

/** \brief Cleanup thread */
void CanServer::cleanupThread()
{
    // Thread loop
    while (true)
    {
        // Cleanup every second
        this_thread::sleep_for(chrono::milliseconds(1000u));

        // Check if new disconnected clients
        {
            const lock_guard<mutex> guard(m_mutex);
            if (m_disconnected_clients.size() != 0)
            {
                for (size_t i = 0; i < m_disconnected_clients.size(); i++)
                {
                    delete m_disconnected_clients[i];
                }
                m_disconnected_clients.clear();
            }
        }
    }
}
