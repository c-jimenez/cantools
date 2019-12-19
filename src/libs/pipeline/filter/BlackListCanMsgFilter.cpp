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

#include "BlackListCanMsgFilter.h"

using namespace std;

/** \brief Constructor */
BlackListCanMsgFilter::BlackListCanMsgFilter()
: CanPipelineStageBase(true)
, m_black_list_std()
, m_black_list_ext()
{}

/** \brief Destructor */
BlackListCanMsgFilter::~BlackListCanMsgFilter()
{}

/** \brief Add a CAN message id to the black list */
void BlackListCanMsgFilter::add(const uint32_t id, const bool extended)
{
    if (extended)
    {
        m_black_list_ext.insert(id);
    }
    else
    {
        m_black_list_std.insert(id);
    }
}

/** \brief Add a range of CAN message ids to the black list */
void BlackListCanMsgFilter::add(const CanMsgIdRange& range, const bool extended)
{
    black_list_t* black_list;
    if (extended)
    {
        black_list = &m_black_list_ext;
    }
    else
    {
        black_list = &m_black_list_std;
    }
    for (uint32_t id = range.minId(); id <= range.maxId(); id++)
    {
        black_list->insert(id);
    }
}

/** \brief Remove a CAN message id from the black list */
void BlackListCanMsgFilter::remove(const uint32_t id, const bool extended)
{
    if (extended)
    {
        m_black_list_ext.erase(id);
    }
    else
    {
        m_black_list_std.erase(id);
    }
}

/** \brief Remove a range of CAN message ids from the black list */
void BlackListCanMsgFilter::remove(const CanMsgIdRange& range, const bool extended)
{
    black_list_t* black_list;
    if (extended)
    {
        black_list = &m_black_list_ext;
    }
    else
    {
        black_list = &m_black_list_std;
    }
    for (uint32_t id = range.minId(); id <= range.maxId(); id++)
    {
        black_list->erase(id);
    }
}


/** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
bool BlackListCanMsgFilter::processCanData(const CanData& can_data)
{
    bool forward_data = true;

    if (CAN_DATA_IS_CANMSG(can_data))
    {
        black_list_t* black_list;
        if (can_data.msg.extended)
        {
            black_list = &m_black_list_ext;
        }
        else
        {
            black_list = &m_black_list_std;
        }

        forward_data = (black_list->find(can_data.msg.id) == black_list->end());
    }

    return forward_data;
}
