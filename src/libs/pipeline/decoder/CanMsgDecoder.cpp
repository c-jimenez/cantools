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

#include "CanMsgDecoder.h"
#include "ICanMsgDatabase.h"

using namespace std;

/** \brief Constructor */
CanMsgDecoder::CanMsgDecoder()
: CanPipelineStageBase(false)
, m_can_msg_database(nullptr)
, m_listener(nullptr)
{}

/** \brief Destructor */
CanMsgDecoder::~CanMsgDecoder()
{}


/** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
bool CanMsgDecoder::processCanData(const CanData& can_data)
{
    // Decode only received CAN messages
    if (can_data.header.type == CAN_DATA_RX_MSG)
    {
        if (m_can_msg_database != nullptr)
        {
            // Look for the message in the database
            auto can_msgs = m_can_msg_database->getCanMsgs();
            auto iter = can_msgs.find(can_data.msg.id);
            if (iter != can_msgs.end())
            {
                // Update message signals
                auto can_msg_desc = iter->second;
                can_msg_desc->updateSignals(can_data.msg);

                // Notify listener
                if (m_listener != nullptr)
                {
                    m_listener->canMsgDecoded(can_data.msg, *can_msg_desc);
                }
            }
        }
    }

    return false;
}
