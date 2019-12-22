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

#include "CanMsgForwarder.h"
#include "CanController.h"

using namespace std;

/** \brief Constructor */
CanMsgForwarder::CanMsgForwarder()
: CanPipelineStageBase(false)
, m_can_controller(nullptr)
{}

/** \brief Destructor */
CanMsgForwarder::~CanMsgForwarder()
{}


/** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
bool CanMsgForwarder::processCanData(const CanData& can_data)
{
    // Forward only received CAN messages
    if (can_data.header.type == CAN_DATA_RX_MSG)
    {
        if (m_can_controller != nullptr)
        {
            m_can_controller->send(can_data.msg);
        }
    }

    return false;
}
