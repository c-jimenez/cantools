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

#include "CanMsgDesc.h"
#include "CanMsgSignal.h"
#include "ICanMsgValue.h"

#include <algorithm>
using namespace std;


/** \brief Constructor */
CanMsgDesc::CanMsgDesc(const uint32_t id, const std::string& name, const bool extended)
: m_id(id)
, m_name(name)
, m_extended(extended)
, m_values()
{}

/** \brief Destructor */
CanMsgDesc::~CanMsgDesc()
{}

/** \brief Add a message's value */
bool CanMsgDesc::addValue(std::shared_ptr<ICanMsgValue>& msg_value)
{
    bool ret = false;
    
    if (msg_value)
    {
        // Check if the value overlap an existing value inside the message
        bool overlap = false;
        for (size_t i = 0; !overlap && (i < m_values.size()); i++)
        {
            const ICanMsgValue& value = *(m_values[i]);
            overlap = (((msg_value->getStartBitPosition() >= value.getStartBitPosition()) && (msg_value->getStartBitPosition() <= value.getEndBitPosition())) ||
                       ((msg_value->getEndBitPosition() >= value.getStartBitPosition()) && (msg_value->getEndBitPosition() <= value.getEndBitPosition())));
        }
        ret = !overlap;
        if (ret)
        {
            m_values.push_back(msg_value);
            msg_value->setCanMsg(*this);
        }
    }

    return ret;
}

/** \brief Add a message's signal */
bool CanMsgDesc::addSignal(std::shared_ptr<CanMsgSignal>& msg_signal)
{
    bool ret = false;

    if (msg_signal)
    {
        m_signals.push_back(msg_signal);
        ret = true;
    }

    return ret;
}

/** \brief Update the signals from a CAN message */
void CanMsgDesc::updateSignals(const CanMsg& can_msg)
{
    auto update = [can_msg](shared_ptr<CanMsgSignal>& signal) { signal->updateValues(can_msg); };
    std::for_each(m_signals.begin(), m_signals.end(), update);
}
