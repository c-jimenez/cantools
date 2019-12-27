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

#include "DecodedCanMsgStreamLogger.h"
#include "CanMsgDesc.h"
#include "CanMsgSignal.h"

#include <sstream>
#include <algorithm>
#include <time.h>
using namespace std;

/** \brief Constructor */
DecodedCanMsgStreamLogger::DecodedCanMsgStreamLogger()
: m_output_stream(nullptr)
, m_output_mode(OutputMode::OUT_TEXT)
{}

/** \brief Destructor */
DecodedCanMsgStreamLogger::~DecodedCanMsgStreamLogger()
{}


/** \brief Set the output options */
void DecodedCanMsgStreamLogger::setOutputOptions(std::ostream& output_stream, const OutputMode output_mode)
{
    m_output_stream = &output_stream;
    m_output_mode = output_mode;
}

/** \brief Called when message has been decoded */
void DecodedCanMsgStreamLogger::canMsgDecoded(const CanMsg& can_msg, const CanMsgDesc& decoded_can_msg)
{
    // Check if the output stream has been configured
    if (m_output_stream != nullptr)
    {
        // Format output
        switch (m_output_mode)
        {
            case OUT_TEXT:
            {
                textOutput(can_msg, decoded_can_msg);
                break;
            }

            case OUT_CSV:
            {
                csvOutput(can_msg, decoded_can_msg);
                break;
            }

            default:
            {
                // Ignore
                break;
            }
        }
    }
}

/** \brief Output data in text mode */
void DecodedCanMsgStreamLogger::textOutput(const CanMsg& can_msg, const CanMsgDesc& decoded_can_msg)
{
    std::ostream& output_stream = *m_output_stream;

    stringstream timestamp_str;
    struct tm timestamp = { 0 };
    localtime_r(&can_msg.header.timestamp_sec, &timestamp);
    timestamp_str << std::dec;
    timestamp_str << (timestamp.tm_year + 1900) << "/" << (timestamp.tm_mon + 1) << "/" << timestamp.tm_mday << " - ";
    timestamp_str << timestamp.tm_hour << ":" << timestamp.tm_min << ":" << timestamp.tm_sec << ".";
    timestamp_str << can_msg.header.timestamp_nsec << " - ";

    auto signals = decoded_can_msg.getSignals();
    auto print_signal = [this, can_msg, &output_stream, &timestamp_str](const shared_ptr<CanMsgSignal>& signal)
    {
        output_stream << timestamp_str.str();
        if (can_msg.header.type == CAN_DATA_RX_MSG)
        {
            output_stream << "RX - ";
        }
        else
        {
            output_stream << "TX - ";
        }
        output_stream << signal->getName() << " - ";
        printValue(signal->getConvertedValue(), signal->getConvertedValueType());
        if (!signal->getUnit().empty())
        {
            output_stream << " " << signal->getUnit();
        }
        output_stream << " (";
        printValue(signal->getRawValue(), signal->getRawValueType());
        output_stream << ")";

        output_stream << endl;
    };

    for_each(signals.cbegin(), signals.cend(), print_signal);
}

/** \brief Output data in CSV mode */
void DecodedCanMsgStreamLogger::csvOutput(const CanMsg& can_msg, const CanMsgDesc& decoded_can_msg)
{
    std::ostream& output_stream = *m_output_stream;

    stringstream timestamp_str;
    struct tm timestamp = { 0 };
    localtime_r(&can_msg.header.timestamp_sec, &timestamp);
    timestamp_str << std::dec;
    timestamp_str << (timestamp.tm_year + 1900) << "/" << (timestamp.tm_mon + 1) << "/" << timestamp.tm_mday << " - ";
    timestamp_str << timestamp.tm_hour << ":" << timestamp.tm_min << ":" << timestamp.tm_sec << ".";
    timestamp_str << can_msg.header.timestamp_nsec << ";";

    auto signals = decoded_can_msg.getSignals();
    auto print_signal = [this, can_msg, &output_stream, &timestamp_str](const shared_ptr<CanMsgSignal>& signal)
    {
        output_stream << timestamp_str.str();
        if (can_msg.header.type == CAN_DATA_RX_MSG)
        {
            output_stream << "RX;";
        }
        else
        {
            output_stream << "TX;";
        }
        output_stream << signal->getName() << ";";
        printValue(signal->getConvertedValue(), signal->getConvertedValueType());
        if (!signal->getUnit().empty())
        {
            output_stream << ";" << signal->getUnit();
        }
        output_stream << ";";
        printValue(signal->getRawValue(), signal->getRawValueType());

        output_stream << endl;
    };

    for_each(signals.cbegin(), signals.cend(), print_signal);
}

/** \brief Outputs a value to the stream */
void DecodedCanMsgStreamLogger::printValue(const void* value, const std::string& value_type)
{
    std::ostream& output_stream = *m_output_stream;

    if (value_type == "uint")
    {
        output_stream << *reinterpret_cast<const uint64_t*>(value);
    }
    else if (value_type == "int")
    {
        output_stream << *reinterpret_cast<const int64_t*>(value);
    }
    else if (value_type == "double")
    {
        output_stream << *reinterpret_cast<const double*>(value);
    }
    else if (value_type == "float")
    {
        output_stream << *reinterpret_cast<const float*>(value);
    }
    else if (value_type == "string")
    {
        output_stream << *reinterpret_cast<const string*>(value);
    }
    else
    {
        output_stream << "Unknown type (" << value_type << ")";
    }
}
