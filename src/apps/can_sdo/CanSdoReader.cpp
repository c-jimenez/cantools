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

#include "CanSdoReader.h"
#include "CanSdoAbortCode.h"
#include "CanController.h"

#include "osal.h"

#include <sstream>
#include <thread>
#include <chrono>
using namespace std;

/** \brief Constructor */
CanSdoReader::CanSdoReader(CanController& controller)
: CanPipelineStageBase(false)
, m_controller(controller)
{}

/** \brief Destructor */
CanSdoReader::~CanSdoReader()
{}


/** \brief Start the read operation */
bool CanSdoReader::read(const uint16_t client_sdo, const uint16_t server_sdo, const uint16_t obd_index, const uint8_t obd_subindex)
{
    bool ret = true;

    // Save parameters
    m_client_sdo = client_sdo;
    m_server_sdo = server_sdo;
    m_obd_index = obd_index;
    m_obd_subindex = obd_subindex;

    // Upload state
    m_segment_number = 0;
    m_data_size = 0;
    m_data_index = 0;
    memset(m_data, 0, sizeof(m_data));
    m_transfer_complete = false;
    m_transfer_error = false;
    m_started = false;
    m_last_error = "";

    // Send SDO upload initiate
    CanMsg can_msg = { 0 };
    can_msg.id = m_server_sdo;
    can_msg.size = 8u;
    can_msg.data[0] = (2u << 5u);
    can_msg.data[1] = static_cast<uint8_t>(obd_index & 0xFFu);
    can_msg.data[2] = static_cast<uint8_t>((obd_index >> 8u) & 0xFFu);
    can_msg.data[3] = obd_subindex;
    ret = m_controller.send(can_msg);
    if (ret)
    {
        m_timeout = 0;
        while (!m_transfer_complete && !m_transfer_error && (m_timeout != 1000u))
        {
            this_thread::sleep_for(chrono::milliseconds(100u));
            m_timeout += 100;
        }
        if (m_timeout == 1000)
        {
            m_last_error = "SDO transfer timeout";
            ret = false;
        }
        else
        {
            ret = !m_transfer_error;
        }
        if (ret && (m_data_size == 0))
        {
            m_data_size = m_data_index;
        }
    }
    else
    {
        m_last_error = m_controller.getErrorMsg();
    }

    return ret;
}


/** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
bool CanSdoReader::processCanData(const CanData& can_data)
{
    if (can_data.header.type == CAN_DATA_RX_MSG)
    {
        const CanMsg& can_msg = can_data.msg;

        // Handle only SDO responses
        if (can_msg.id == m_client_sdo)
        {
            // Reset timeout 
            m_timeout = 0;

            if (!m_started)
            {
                // Decode upload initiate response
                uint8_t scs = can_msg.data[0] >> 5u;
                uint8_t n = (can_msg.data[0] >> 2u) & 0x03u;
                bool expedited = ((can_msg.data[0] & (1u << 1u)) != 0) ? true : false;
                bool s = ((can_msg.data[0] & (1u << 0u)) != 0) ? true : false;
                if (scs == 2)
                {
                    if (!expedited && s)
                    {
                        m_data_size = can_msg.data[4] + (can_msg.data[5] << 8u) + (can_msg.data[6] << 16u) + (can_msg.data[7] << 24u);
                    }
                    else if (expedited && s)
                    {
                        m_data_size = 4 - n;
                        memcpy(m_data, &can_msg.data[4], m_data_size);
                        m_transfer_complete = true;
                    }
                    else if (expedited && !s)
                    {
                        m_data_size = 0;
                    }
                    else
                    {
                        m_transfer_error = true;

                        stringstream ss;
                        ss << "Invalid SDO response => e = " << expedited << ", s = " << (int)s;
                        m_last_error = ss.str();
                    }
                }
                else
                {
                    m_transfer_error = true;

                    if (scs == 4)
                    {
                        m_last_error = "SDO transfer abort received : " + CANSDOABORTCODE_GetAbortString(can_msg);
                    }
                    else
                    {
                        stringstream ss;
                        ss << "Invalid SDO response => scs = " << (int)scs;
                        m_last_error = ss.str();
                    }
                }
            }
            else
            {
                // Decode upload segment response
                uint8_t scs = can_msg.data[0] >> 5u;
                uint8_t t = (can_msg.data[0] >> 4u) & 0x01u;
                uint8_t n = (can_msg.data[0] >> 1u) & 0x07u;
                bool c = ((can_msg.data[0] & (1u << 0u)) != 0) ? true : false;
                if (scs == 0)
                {
                    if (t == (m_segment_number & 0x01))
                    {
                        uint8_t size = 7u - n;
                        memcpy(&m_data[m_data_index], &can_msg.data[1], size);
                        m_data_index += size;
                        m_transfer_complete = c;
                        m_segment_number++;
                    }
                    else
                    {
                        m_transfer_error = true;

                        stringstream ss;
                        ss << "Invalid SDO response => t = " << (int)t;
                        m_last_error = ss.str();
                    }
                }
                else
                {
                    m_transfer_error = true;

                    if (scs == 4)
                    {
                        m_last_error = "SDO transfer abort received : " + CANSDOABORTCODE_GetAbortString(can_msg);
                    }
                    else
                    {
                        stringstream ss;
                        ss << "Invalid SDO response => scs = " << scs;
                        m_last_error = ss.str();
                    }
                }
            }
            if (!m_transfer_error && !m_transfer_complete)
            {
                // Request next segment
                CanMsg can_msg = { 0 };
                can_msg.id = m_server_sdo;
                can_msg.size = 8u;
                can_msg.data[0] = (3u << 5u) | ((m_segment_number & 0x01) << 4u);
                m_transfer_error = !m_controller.send(can_msg);
                m_started = true;
            }
        }
    }

    return false;
}
