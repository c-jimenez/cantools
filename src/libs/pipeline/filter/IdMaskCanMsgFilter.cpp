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

#include "IdMaskCanMsgFilter.h"

using namespace std;

/** \brief Constructor */
IdMaskCanMsgFilter::IdMaskCanMsgFilter()
: CanPipelineStageBase(true)
, m_filters_std()
, m_filters_ext()
{}

/** \brief Destructor */
IdMaskCanMsgFilter::~IdMaskCanMsgFilter()
{}

/** \brief Add a message id mask filter */
void IdMaskCanMsgFilter::add(const uint32_t id, const uint32_t mask, const bool extended)
{
    if (extended)
    {
        m_filters_ext.push_back(pair<uint32_t, uint32_t>(id, mask));
    }
    else
    {
        m_filters_std.push_back(pair<uint32_t, uint32_t>(id, mask));
    }
}

/** \brief Remove a message id mask filter */
void IdMaskCanMsgFilter::remove(const uint32_t id, const uint32_t mask, const bool extended)
{
    if (extended)
    {
        m_filters_ext.remove(pair<uint32_t, uint32_t>(id, mask));
    }
    else
    {
        m_filters_std.remove(pair<uint32_t, uint32_t>(id, mask));
    }
}


/** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
bool IdMaskCanMsgFilter::processCanData(const CanData& can_data)
{
    bool forward_data = true;

    if (CAN_DATA_IS_CANMSG(can_data))
    {
        filter_mask_list_t* filters;
        if (can_data.msg.extended)
        {
            filters = &m_filters_ext;
        }
        else
        {
            filters = &m_filters_std;
        }

        for (auto iter = filters->begin(); (forward_data && (iter != filters->end())); ++iter)
        {
            const uint32_t id = iter->first;
            const uint32_t mask = iter->second;
            forward_data = ((can_data.msg.id & mask) != id);
        }
    }

    return forward_data;
}
