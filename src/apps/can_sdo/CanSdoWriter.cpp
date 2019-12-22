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

#include "CanSdoWriter.h"
#include "CanController.h"
#include "CanSdoAbortCode.h"

#include "osal.h"

#include <sstream>
#include <thread>
#include <chrono>
using namespace std;

/** \brief Constructor */
CanSdoWriter::CanSdoWriter(CanController& controller)
: CanPipelineStageBase(false)
, m_controller(controller)
{}

/** \brief Destructor */
CanSdoWriter::~CanSdoWriter()
{}


/** \brief Start the write operation */
bool CanSdoWriter::write(const uint16_t client_sdo, const uint16_t server_sdo, const uint16_t obd_index, const uint8_t obd_subindex, const uint8_t* data, const uint32_t size)
{
    bool ret = true;

    // Save parameters
    m_client_sdo = client_sdo;
    m_server_sdo = server_sdo;
    m_obd_index = obd_index;
    m_obd_subindex = obd_subindex;

    // Upload state
    m_segment_number = 0;
    m_data_size = size;
    m_data_index = 0;
    m_data = data;
    m_transfer_complete = false;
    m_transfer_error = false;
    m_started = false;
    m_last_error = "";

    // Send SDO download initiate
    CanMsg can_msg = { 0 };
    can_msg.id = m_server_sdo;
    can_msg.size = 8u;
    can_msg.data[0] = (1u << 5u) | (1u << 0);
    can_msg.data[1] = static_cast<uint8_t>(obd_index & 0xFFu);
    can_msg.data[2] = static_cast<uint8_t>((obd_index >> 8u) & 0xFFu);
    can_msg.data[3] = obd_subindex;
    can_msg.data[4] = static_cast<uint8_t>(size & 0xFFu);
    can_msg.data[5] = static_cast<uint8_t>((size >> 8u) & 0xFFu);
    can_msg.data[6] = static_cast<uint8_t>((size >> 16u) & 0xFFu);
    can_msg.data[7] = static_cast<uint8_t>((size >> 24u) & 0xFFu);
    ret = m_controller.send(can_msg);
    if (ret)
    {
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
    }
    else
    {
        m_last_error = m_controller.getErrorMsg();
    }

    return ret;
}


/** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
bool CanSdoWriter::processCanData(const CanData& can_data)
{
    if (CAN_DATA_IS_CANMSG(can_data))
    {
        const CanMsg& can_msg = can_data.msg;

        // Handle only SDO responses
        if (can_msg.id == m_client_sdo)
        {
            // Reset timeout 
            m_timeout = 0;

            if (!m_started)
            {
                // Decode download initiate response
                uint8_t scs = can_msg.data[0] >> 5u;
                if (scs != 3)
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
                // Decode download segment response
                uint8_t scs = can_msg.data[0] >> 5u;
                uint8_t t = (can_msg.data[0] >> 4u) & 0x01u;
                if (scs == 1)
                {
                    if (t == (m_segment_number & 0x01))
                    {
                        m_transfer_complete = (m_data_index == m_data_size);
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
                // Compute next segment size
                uint8_t n = 0;
                uint8_t c = 0;
                uint16_t size = m_data_size - m_data_index;
                if (size > 7u)
                {
                    size = 7;
                }
                else
                {
                    n = 7 - size;
                    c = 1;
                }

                // Download next segment
                CanMsg can_msg = { 0 };
                can_msg.id = m_server_sdo;
                can_msg.size = 8u;
                can_msg.data[0] = (0u << 5u) | ((m_segment_number & 0x01) << 4u) | (n << 1u) | (c << 0u);
                memcpy(&can_msg.data[1], &m_data[m_data_index], size);
                m_transfer_error = !m_controller.send(can_msg);
                m_started = true;
                m_data_index += size;
            }
        }
    }

    return false;
}
