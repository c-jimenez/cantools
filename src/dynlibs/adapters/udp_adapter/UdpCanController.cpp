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

#include "UdpCanController.h"
#include "ICanControllerListener.h"

#include "osal.h"

#include <limits>
using namespace std;

/** \brief Constructor */
UdpCanController::UdpCanController(const std::string& ip, const uint16_t port)
: m_ip(ip)
, m_port(port)
, m_last_error("")
, m_caps()
, m_socket()
, m_serializer(m_socket, 250u)
, m_stop_rx_thread(false)
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
UdpCanController::~UdpCanController()
{
    close();
}


/** \brief Open the controller */
bool UdpCanController::open()
{
    bool ret = false;
    
    if (OSAL_Init())
    {
        if (!m_opened)
        {
            // Open UDP socket
            ret = m_socket.open(m_ip, m_port);
            if (ret)
            {
                m_opened = true;
            }
            else
            {
                m_last_error = "Unable to open multicast UDP socket";
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
bool UdpCanController::close()
{
    bool ret = false;

    if (m_opened)
    {
        if (m_started)
        {
            // Stop the controller
            stop();
        }

        // Close the socket
        m_socket.close();
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
bool UdpCanController::configure(const CanBitrate bitrate, const uint8_t mode)
{
    // Nothing to do
    (void)bitrate;
    (void)mode;
    return true;
}

/** \brief Get the controller's capababilities */
bool UdpCanController::getCapabilities(CanAdapterControllerCapabilities* caps) const
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
bool UdpCanController::start(const CanAdapterControllerCallbacks* callbacks)
{
    bool ret = false;

    // Check if the connection is opened
    if (m_opened)
    {
        if (!m_started)
        {
            // Flush Rx
            UdpCanSerializer::UdpMsg udp_msg = { 0 };
            UdpCanSerializer::UdpMsgType type = UdpCanSerializer::CAN_MSG;
            while (m_serializer.read(udp_msg, type))
            {}

            // Start Rx thread
            m_callbacks = *callbacks;
            m_stop_rx_thread = false;
            m_rx_thread = new thread(&UdpCanController::rxThread, this);
            m_started = true;
            ret = true;
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
bool UdpCanController::stop()
{
    bool ret = false;

    if (m_started)
    {
        // Stop the server
        m_stop_rx_thread = true;

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
bool UdpCanController::send(const CanMsg* can_msg)
{
    bool ret = false;

    // Check if started
    if (m_started && (can_msg != nullptr))
    {
        // Send message
        ret = m_serializer.write(*can_msg);
        if (!ret)
        {
            m_last_error = "Unable to send data throug the socket";
        }
    }

    return ret;
}

/** \brief Rx thread */
void UdpCanController::rxThread()
{
    while (!m_stop_rx_thread)
    {
        // Wait for data
        UdpCanSerializer::UdpMsg udp_msg = { 0 };
        UdpCanSerializer::UdpMsgType type = UdpCanSerializer::CAN_MSG;
        bool ret = m_serializer.read(udp_msg, type);
        if (ret && !m_stop_rx_thread)
        {
            switch (type)
            {
                case UdpCanSerializer::CAN_MSG:
                {
                    // CAN message received
                    CanData can_data = { 0 };
                    can_data.msg = udp_msg.can_msg;
                    can_data.header.type = CAN_DATA_RX_MSG;
                    OSAL_GetTimestamp(&can_data.header.timestamp_sec, &can_data.header.timestamp_nsec);

                    // Notify message
                    m_callbacks.data_received(m_callbacks.parameter, &can_data);
                    break;
                }

                case UdpCanSerializer::CAN_ERR_FRAME:
                    // Intended fallthrough
                default:
                {
                    // Error frame
                    CanData can_data = { 0 };
                    can_data.header.type = CAN_DATA_EVENT;
                    can_data.event.type = CANEVT_ERROR;
                    OSAL_GetTimestamp(&can_data.header.timestamp_sec, &can_data.header.timestamp_nsec);

                    // Notify message
                    m_callbacks.data_received(m_callbacks.parameter, &can_data);
                    break;
                }
            }
        }
    }
}
