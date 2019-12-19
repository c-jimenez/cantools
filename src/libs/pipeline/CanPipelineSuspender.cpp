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

#include "CanPipelineSuspender.h"
#include "VectorHelper.h"

using namespace std;

/** \brief Constructor */
CanPipelineSuspender::CanPipelineSuspender()
: CanPipelineStageBase(true)
, m_is_suspended(false)
, m_mutex()
, m_can_data()
{}

/** \brief Destructor */
CanPipelineSuspender::~CanPipelineSuspender()
{}

/** \brief Suspend message reception */
void CanPipelineSuspender::suspend()
{
    m_is_suspended = true;
}

/** \brief Resume message reception */
void CanPipelineSuspender::resume()
{
    const lock_guard<mutex> guard(m_mutex);

    size_t i = 0;
    while (i < m_can_data.size())
    {
        CanData& can_data = m_can_data[i];
        dispatchCanData(can_data);
        i++;
    }
    m_can_data.clear();
    m_is_suspended = false;
}

/** \brief Clear the message queue */
void CanPipelineSuspender::clear()
{
    const lock_guard<mutex> guard(m_mutex);

    bool is_suspent = m_is_suspended;
    if (!is_suspent)
    {
        suspend();
    }
    m_can_data.clear();
    if (!is_suspent)
    {
        resume();
    }
}

/** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
bool CanPipelineSuspender::processCanData(const CanData& can_data)
{
    bool forward_data = true;
 
    if (m_is_suspended)
    {
        const lock_guard<mutex> guard(m_mutex);
        if (m_is_suspended)
        {
            m_can_data.push_back(can_data);
            forward_data = false;
        }
    }

    return forward_data;
}
