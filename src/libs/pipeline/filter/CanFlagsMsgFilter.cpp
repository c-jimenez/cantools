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

#include "CanFlagsMsgFilter.h"


/** \brief Constructor */
CanFlagsMsgFilter::CanFlagsMsgFilter(const uint8_t acceptance_mask)
: CanPipelineStageBase(true)
, m_acceptance_flags(acceptance_mask)
{}

/** \brief Destructor */
CanFlagsMsgFilter::~CanFlagsMsgFilter()
{}

/** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
bool CanFlagsMsgFilter::processCanData(const CanData& can_data)
{
    bool forward_data = true;

    if (CAN_DATA_IS_CANMSG(can_data))
    {
        if ((m_acceptance_flags & CANMSG_FLAG_STD) != 0)
        {
            forward_data = !can_data.msg.extended;
        }
        if ((m_acceptance_flags & CANMSG_FLAG_EXT) != 0)
        {
            forward_data = forward_data || can_data.msg.extended;
        }
        if ((m_acceptance_flags & CANMSG_FLAG_RTR) != 0)
        {
            forward_data = forward_data || can_data.msg.rtr;
        }
    }

    return forward_data;
}
