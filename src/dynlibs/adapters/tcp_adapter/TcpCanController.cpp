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

#include "TcpCanController.h"
#include "ICanControllerListener.h"

#include "osal.h"

using namespace std;

/** \brief Constructor */
TcpCanController::TcpCanController(const std::string& server, const uint16_t port, const uint32_t timeout)
: m_server(server)
, m_port(port)
, m_timeout(timeout)
, m_last_error("")
, m_caps()
, m_tcp_client()
, m_serializer(m_tcp_client, timeout)
, m_rx_thread(nullptr)
, m_callbacks()
, m_opened(false)
, m_started(false)
{
    m_caps.extended = true;
    m_caps.max_bitrate = static_cast<uint16_t>(CANBR_1000K);
    m_caps.bus_load = false;
    m_caps.no_ack = false;    
}

/** \brief Destructor */
TcpCanController::~TcpCanController()
{
    close();
}


/** \brief Open the controller */
bool TcpCanController::open()
{
    bool ret = false;
    
    if (OSAL_Init())
    {
        if (!m_opened)
        {
            // Connect to the TCP server
            ret = m_tcp_client.connect(m_server, m_port, m_timeout);
            if (ret)
            {
                m_opened = true;
            }
            else
            {
                m_last_error = "Unable to connect to TCP server";
            }
        }
        else
        {
            m_last_error = "Controller already opened";
        }
    }
    else
    {
        m_last_error = "Unable to initialize OSAL";
    }

    return ret;
}

/** \brief Close the controller */
bool TcpCanController::close()
{
    bool ret = false;

    if (m_opened)
    {
        if (m_started)
        {
            // Stop the controller
            stop();
        }

        // Disconnect from the TCP server
        m_tcp_client.close();
        m_opened = false;
        ret = true;
    }
    else
    {
        m_last_error = "Controller not opened";
    }

    return ret;
}

/** \brief Configure the controller */
bool TcpCanController::configure(const CanBitrate bitrate, const uint8_t mode)
{
    // Nothing to do
    (void)bitrate;
    (void)mode;
    return true;
}

/** \brief Get the controller's capababilities */
bool TcpCanController::getCapabilities(CanAdapterControllerCapabilities* caps) const
{
    bool ret = false;

    if (caps != nullptr)
    {
        *caps = m_caps;
        ret = true;
    }
    
    return ret;
}


/** \brief Start the controller */
bool TcpCanController::start(const CanAdapterControllerCallbacks* callbacks)
{
    bool ret = false;

    // Check if the connection is opened
    if (m_opened)
    {
        if (!m_started)
        {
            // Send start request
            const uint16_t cmd = TcpCanSerializer::CMD_START;
            ret = m_serializer.write(cmd);
            if (ret)
            {
                // Wait response
                uint16_t response = 0;
                ret = m_serializer.read(response);
                if (ret)
                {
                    // Check response
                    ret = (response == cmd);
                    if (ret)
                    {
                        // Start Rx thread
                        m_callbacks = *callbacks;
                        m_rx_thread = new thread(&TcpCanController::rxThread, this);
                        m_started = ret;
                    }
                    else
                    {
                        m_last_error = "Invalid response from server";
                    }
                }
                else
                {
                    if (m_tcp_client.isConnected())
                    {
                        m_last_error = "No response from server";
                    }
                    else
                    {
                        m_last_error = "Disconnected from server";
                    }
                }
            }
            else
            {
                if (m_tcp_client.isConnected())
                {
                    m_last_error = "Unable to send data to the server";
                }
                else
                {
                    m_last_error = "Disconnected from server";
                }
            }
        }
        else
        {
            m_last_error = "Controller already started";
        }
    }
    else
    {
        m_last_error = "Controller not opened";
    }

    return ret;
}

/** \brief Stop the controller */
bool TcpCanController::stop()
{
    bool ret = false;

    if (m_started)
    {
        // Check if connected to the server
        if (m_tcp_client.isConnected())
        {
            // Send stop request
            const uint16_t cmd = TcpCanSerializer::CMD_STOP;
            m_serializer.write(cmd);
        }

        // Wait for the end of the Rx thread
        m_rx_thread->join();

        m_started = false;
        ret = true;
    }
    else
    {
        m_last_error = "Controller not started";
    }

    return ret;
}

/** \brief Send a message through the controller */
bool TcpCanController::send(const CanMsg* can_msg)
{
    bool ret = false;

    // Check if started
    if (m_started && (can_msg != nullptr))
    {
        // Send message
        ret = m_serializer.write(*can_msg);
        if (!ret)
        {
            if (m_tcp_client.isConnected())
            {
                m_last_error = "Unable to send data to the server";
            }
            else
            {
                m_last_error = "Disconnected from server";
            }
        }
    }

    return ret;
}

/** \brief Rx thread */
void TcpCanController::rxThread()
{
    bool ret = true;
    bool stop = false;
    uint16_t cmd = 0;
    while (ret && !stop)
    {
        // Wait for command
        cmd = 0;
        ret = m_serializer.read(cmd);
        if (ret)
        {
            switch (cmd)
            {
                case TcpCanSerializer::CMD_STOP:
                {
                    // Stop controller
                    stop = true;
                    break;
                }

                case TcpCanSerializer::CMD_MSG:
                {
                    // CAN message received
                    CanData can_data = { 0 };
                    can_data.header.type = CAN_DATA_RX_MSG;
                    ret = m_serializer.read(can_data.msg);
                    if (ret)
                    {
                        // Add timestamp
                        OSAL_GetTimestamp(&can_data.header.timestamp_sec, &can_data.header.timestamp_nsec);

                        // Notify message
                        m_callbacks.data_received(m_callbacks.parameter, &can_data);
                    }
                    break;
                }

                case TcpCanSerializer::CMD_ERR_FRAME:
                {
                    // Error frame
                    CanData can_data = { 0 };
                    can_data.header.type = CAN_DATA_EVENT;
                    OSAL_GetTimestamp(&can_data.header.timestamp_sec, &can_data.header.timestamp_nsec);
                    can_data.event.type = CANEVT_ERROR;
                    m_callbacks.data_received(m_callbacks.parameter, &can_data);
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
            ret = m_tcp_client.isConnected();
            if (!ret && !stop)
            {
                // Disconnected from server
                CanData can_data = { 0 };
                can_data.header.type = CAN_DATA_EVENT;
                OSAL_GetTimestamp(&can_data.header.timestamp_sec, &can_data.header.timestamp_nsec);
                can_data.event.type = CANEVT_CONTROLLER_ERROR;
                m_last_error = "Disconnected from server";
                m_callbacks.data_received(m_callbacks.parameter, &can_data);
            }
        }
    }
}
