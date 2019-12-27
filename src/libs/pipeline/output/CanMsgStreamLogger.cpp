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

#include "CanMsgStreamLogger.h"
#include "CanController.h"
#include "osal.h"

using namespace std;

/** \brief Constructor */
CanMsgStreamLogger::CanMsgStreamLogger()
: CanPipelineStageBase(false)
, m_output_stream(nullptr)
, m_output_mode(OutputMode::OUT_TEXT)
, m_can_controller(nullptr)
{}

/** \brief Destructor */
CanMsgStreamLogger::~CanMsgStreamLogger()
{}


/** \brief Set the output options */
void CanMsgStreamLogger::setOutputOptions(std::ostream& output_stream, const OutputMode output_mode, const CanController& can_controller)
{
    m_output_stream = &output_stream;
    m_output_mode = output_mode;
    m_can_controller = &can_controller;
}

/** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
bool CanMsgStreamLogger::processCanData(const CanData& can_data)
{
    // Check if the output stream has been configured
    if (m_output_stream != nullptr)
    {
        // Format output
        switch (m_output_mode)
        {
            case OUT_TEXT:
            {
                textOutput(can_data);
                break;
            }

            case OUT_CSV:
            {
                csvOutput(can_data);
                break;
            }

            case OUT_BINARY:
            {
                binaryOutput(can_data);
                break;
            }

            default:
            {
                // Ignore
                break;
            }
        }
    }

    return false;
}

/** \brief Output data in text mode */
void CanMsgStreamLogger::textOutput(const CanData& can_data)
{
    std::ostream& output_stream = *m_output_stream;

    struct tm timestamp = { 0 };
    OSAL_LocalTime(can_data.header.timestamp_sec, &timestamp);
    output_stream << std::dec;
    output_stream << (timestamp.tm_year + 1900) << "/" << (timestamp.tm_mon + 1) << "/" << timestamp.tm_mday << " - ";
    output_stream << timestamp.tm_hour << ":" << timestamp.tm_min << ":" << timestamp.tm_sec << ".";
    output_stream << can_data.header.timestamp_nsec << " - ";

    if (CAN_DATA_IS_CANMSG(can_data))
    {
        const CanMsg& can_msg = can_data.msg;

        output_stream << "[CAN_MSG] : ";
        if (can_data.header.type == CAN_DATA_RX_MSG)
        {
            output_stream << "RX, ";
        }
        else
        {
            output_stream << "TX, ";
        }
        output_stream << "id = 0x" << std::hex << can_msg.id << ", ";
        output_stream << "flags = ";
        if (can_msg.extended)
        {
            output_stream << "ext ";
        }
        if (can_msg.rtr)
        {
            output_stream << "rtr ";
        }
        if (!can_msg.extended && !can_msg.rtr)
        {
            output_stream << "none ";
        }
        output_stream << ", ";
        output_stream << "size = " << std::dec << (int)can_msg.size << ", ";
        output_stream << "data = { " << std::hex;
        for (uint8_t i = 0; i < can_msg.size; i++)
        {
            output_stream << "0x" << (int)can_msg.data[i] << " ";
        }
        output_stream << "}";
        output_stream << endl;
    }
    else if (can_data.header.type == CAN_DATA_EVENT)
    {
        const CanEventType& can_event = can_data.event.type;

        output_stream << "[BUS_EVT] : ";
        switch (can_event)
        {
        case CANEVT_BUS_OFF:
        {
            output_stream << "Bus OFF";
            break;
        }
        case CANEVT_BUS_ON:
        {
            output_stream << "Bus ON";
            break;
        }
        case CANEVT_NO_ACK:
        {
            output_stream << "No ack";
            break;
        }
        case CANEVT_ERROR:
        {
            output_stream << "Error frame";
            break;
        }
        case CANEVT_RX_OVR:
        {
            output_stream << "Rx FIFO overrun";
            break;
        }
        case CANEVT_TX_OVR:
        {
            output_stream << "Tx FIFO overrun";
            break;
        }
        case CANEVT_CONTROLLER_ERROR:
        {
            output_stream << "Controller error => " << m_can_controller->getErrorMsg();
            break;
        }

        default:
        {
            output_stream << "Unknown => " << (int)can_event;
            break;
        }
        }
        output_stream << endl;
    }
    else
    {
        output_stream << "[UNKNOWN]" << endl;
    }
}

/** \brief Output data in CSV mode */
void CanMsgStreamLogger::csvOutput(const CanData& can_data)
{
    std::ostream& output_stream = *m_output_stream;

    struct tm timestamp = { 0 };
    OSAL_LocalTime(can_data.header.timestamp_sec, &timestamp);
    output_stream << std::dec;
    output_stream << (timestamp.tm_year + 1900) << "/" << (timestamp.tm_mon + 1) << "/" << timestamp.tm_mday << " - ";
    output_stream << timestamp.tm_hour << ":" << timestamp.tm_min << ":" << timestamp.tm_sec << ".";
    output_stream << can_data.header.timestamp_nsec << ";";

    if (CAN_DATA_IS_CANMSG(can_data))
    {
        const CanMsg& can_msg = can_data.msg;

        output_stream << "CAN_MSG" << ";";
        if (can_data.header.type == CAN_DATA_RX_MSG)
        {
            output_stream << "RX" << ";";
        }
        else
        {
            output_stream << "TX" << ";";
        }
        output_stream << "0x" << std::hex << can_msg.id << ";";
        if (can_msg.extended)
        {
            output_stream << "ext ";
        }
        if (can_msg.rtr)
        {
            output_stream << "rtr ";
        }
        if (!can_msg.extended && !can_msg.rtr)
        {
            output_stream << "none ";
        }
        output_stream << ";";
        output_stream << std::dec << (int)can_msg.size << ";";
        output_stream << std::hex;
        for (uint8_t i = 0; i < can_msg.size; i++)
        {
            output_stream << "0x" << (int)can_msg.data[i] << " ";
        }
        output_stream << endl;
    }
    else if (can_data.header.type == CAN_DATA_EVENT)
    {
        const CanEventType& can_event = can_data.event.type;

        output_stream << "BUS_EVT" << ";";
        switch (can_event)
        {
            case CANEVT_BUS_OFF:
            {
                output_stream << "Bus OFF";
                break;
            }
            case CANEVT_BUS_ON:
            {
                output_stream << "Bus ON";
                break;
            }
            case CANEVT_NO_ACK:
            {
                output_stream << "No ack";
                break;
            }
            case CANEVT_ERROR:
            {
                output_stream << "Error frame";
                break;
            }
            case CANEVT_RX_OVR:
            {
                output_stream << "Rx FIFO overrun";
                break;
            }
            case CANEVT_TX_OVR:
            {
                output_stream << "Tx FIFO overrun";
                break;
            }
            case CANEVT_CONTROLLER_ERROR:
            {
                output_stream << "Controller error => " << m_can_controller->getErrorMsg();
                break;
            }

            default:
            {
                output_stream << "Unknown => " << (int)can_event;
                break;
            }
        }
        output_stream << endl;
    }
    else
    {
        output_stream << "UNKNOWN" << endl;
    }
}

/** \brief Output data in binary mode */
void CanMsgStreamLogger::binaryOutput(const CanData& can_data)
{
    std::ostream& output_stream = *m_output_stream;
    output_stream.write(reinterpret_cast<const char*>(&can_data), sizeof(CanData));
}
