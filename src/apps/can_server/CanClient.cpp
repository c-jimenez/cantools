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

#include "CanClient.h"

#include <iostream>
using namespace std;


/** \brief Global client counter */
uint32_t CanClient::m_id_counter = 0;


/** \brief Constructor */
CanClient::CanClient(TcpClient* tcp_client, const uint32_t timeout)
: m_id(++m_id_counter)
, m_tcp_client(tcp_client)
, m_serializer(*tcp_client, timeout)
, m_thread(nullptr)
, m_mutex()
, m_disconnected_callback()
, m_msg_received_callback()
, m_controller_started(false)
{}

/** \brief Destructor */
CanClient::~CanClient()
{
    if (m_thread != nullptr)
    {
        m_thread->join();
        delete m_thread;
    }
    delete m_tcp_client;
}

/** \brief Start the client */
void CanClient::start(std::function<void(CanClient&)> disconnected_callback,
                      std::function<void(CanClient&, const CanMsg&)> msg_received_callback)
{
    // Save callbacks
    m_disconnected_callback = disconnected_callback;
    m_msg_received_callback = msg_received_callback;

    // Start the communication thread
    m_thread = new thread(&CanClient::commThread, this);
}

/** \brief Send a CAN message to the client */
void CanClient::sendMsg(const CanMsg& can_msg)
{
    // Send message only if the client's controller is started
    if (m_controller_started)
    {
        const lock_guard<mutex> guard(m_mutex);
        m_serializer.write(can_msg);
    }
}

/** \brief Communication thread */
void CanClient::commThread()
{
    // Thread loop
    bool ret;
    uint16_t cmd;
    bool connected = true;
    while (connected)
    {
        // Wait for cmd
        cmd = 0;
        ret = m_serializer.read(cmd);
        if (ret)
        {
            switch (cmd)
            {
                case TcpCanSerializer::CMD_START:
                {
                    // Controller start command
                    if (!m_controller_started)
                    {
                        const lock_guard<mutex> guard(m_mutex);
                        m_serializer.write(cmd);
                        m_controller_started = true;
                    }
                    break;
                }

                case TcpCanSerializer::CMD_STOP:
                {
                    // Controller stop command
                    if (m_controller_started)
                    {
                        const lock_guard<mutex> guard(m_mutex);
                        m_serializer.write(cmd);
                    }
                    break;
                }

                case TcpCanSerializer::CMD_MSG:
                {
                    // CAN message received
                    if (m_controller_started)
                    {
                        CanMsg can_msg = { 0 };
                        ret = m_serializer.read(can_msg);
                        if (ret)
                        {
                            // Notify message
                            m_msg_received_callback(*this, can_msg);
                        }
                    }
                    break;
                }

                default:
                {
                    // Unknown command => ignore
                    break;
                }
            }

        }
        if (!ret)
        {
            connected = m_tcp_client->isConnected();
        }
    }

    // Notify disconnection
    m_controller_started = false;
    m_disconnected_callback(*this);
}
