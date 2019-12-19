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

#include "WhiteListCanMsgFilter.h"

using namespace std;

/** \brief Constructor */
WhiteListCanMsgFilter::WhiteListCanMsgFilter()
: CanPipelineStageBase(true)
, m_white_list_std()
, m_white_list_ext()
{}

/** \brief Destructor */
WhiteListCanMsgFilter::~WhiteListCanMsgFilter()
{}

/** \brief Add a CAN message id to the white list */
void WhiteListCanMsgFilter::add(const uint32_t id, const bool extended)
{
    if (extended)
    {
        m_white_list_ext.insert(id);
    }
    else
    {
        m_white_list_std.insert(id);
    }
}

/** \brief Add a range of CAN message ids to the white list */
void WhiteListCanMsgFilter::add(const CanMsgIdRange& range, const bool extended)
{
    white_list_t* white_list;
    if (extended)
    {
        white_list = &m_white_list_ext;
    }
    else
    {
        white_list = &m_white_list_std;
    }
    for (uint32_t id = range.minId(); id <= range.maxId(); id++)
    {
        white_list->insert(id);
    }
}

/** \brief Remove a CAN message id from the white list */
void WhiteListCanMsgFilter::remove(const uint32_t id, const bool extended)
{
    if (extended)
    {
        m_white_list_ext.erase(id);
    }
    else
    {
        m_white_list_std.erase(id);
    }
}

/** \brief Remove a range of CAN message ids from the white list */
void WhiteListCanMsgFilter::remove(const CanMsgIdRange& range, const bool extended)
{
    white_list_t* white_list;
    if (extended)
    {
        white_list = &m_white_list_ext;
    }
    else
    {
        white_list = &m_white_list_std;
    }
    for (uint32_t id = range.minId(); id <= range.maxId(); id++)
    {
        white_list->erase(id);
    }
}


/** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
bool WhiteListCanMsgFilter::processCanData(const CanData& can_data)
{
    bool forward_data = true;

    if (CAN_DATA_IS_CANMSG(can_data))
    {
        white_list_t* white_list;
        if (can_data.msg.extended)
        {
            white_list = &m_white_list_ext;
        }
        else
        {
            white_list = &m_white_list_std;
        }

        forward_data = (white_list->find(can_data.msg.id) != white_list->end());
    }

    return forward_data;
}
