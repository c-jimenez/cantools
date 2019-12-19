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

#include "CanPipeline.h"
#include "CanController.h"

using namespace std;

/** \brief Constructor */
CanPipeline::CanPipeline(const std::string& name)
: m_name(name)
, m_controller(nullptr)
, m_last_error("")
, m_rx_thread(nullptr)
, m_rx_mailbox()
, m_suspender()
{}

/** \brief Destructor */
CanPipeline::~CanPipeline()
{}


/** \brief Start the pipeline with the specified CAN controller */
bool CanPipeline::start(CanController& controller)
{
    bool ret = false;

    if (m_rx_thread == nullptr)
    {
        m_rx_mailbox.clear();
        m_controller = &controller;
        ret = m_controller->start(*this);
        if (ret)
        {
            m_rx_thread = new thread(&CanPipeline::rxThread, this);
        }
        else
        {
            m_last_error = m_controller->getErrorMsg();
        }
    }
    else
    {
        m_last_error = "Pipeline already started";
    }

    return ret;
}

/** \brief Suspend the pipeline processing */
bool CanPipeline::suspend()
{
    bool ret = false;

    if (m_rx_thread != nullptr)
    {
        if (!m_suspender.isSuspended())
        {
            m_suspender.suspend();
        }
        else
        {
            m_last_error = "Pipeline already suspended";
        }
    }
    else
    {
        m_last_error = "Pipeline not started";
    }

    return ret;
}

/** \brief Resume the pipeline processing */
bool CanPipeline::resume()
{
    bool ret = false;

    if (m_rx_thread != nullptr)
    {
        if (m_suspender.isSuspended())
        {
            m_suspender.resume();
        }
        else
        {
            m_last_error = "Pipeline not suspended";
        }
    }
    else
    {
        m_last_error = "Pipeline not started";
    }

    return ret;
}

/** \brief Stop the pipeline */
bool CanPipeline::stop()
{
    bool ret = false;

    if (m_rx_thread != nullptr)
    {
        ret = m_controller->stop();
        if (!ret)
        {
            m_last_error = m_controller->getErrorMsg();
        }
        m_rx_mailbox.clear(true);
        m_rx_thread->join();
        delete m_rx_thread;
        m_rx_thread = nullptr;
    }
    else
    {
        m_last_error = "Pipeline not started";
    }

    return ret;
}


/** \brief Send a message through the CAN controller of the pipeline */
bool CanPipeline::send(const CanMsg& can_msg)
{
    bool ret = false;

    if (m_rx_thread != nullptr)
    {
        ret = m_controller->send(can_msg);
        if (!ret)
        {
            m_last_error = m_controller->getErrorMsg();
        }
    }
    else
    {
        m_last_error = "Pipeline not started";
    }

    return ret;
}

/** \brief Add a child pipeline stage */
void CanPipeline::addChild(ICanPipelineStage& child)
{
    m_suspender.addChild(child);
}

/** \brief Remove a child pipeline stage */
void CanPipeline::removeChild(ICanPipelineStage& child)
{
    m_suspender.removeChild(child);
}

/** \brief Called when CAN data has been received on the controller */
void CanPipeline::canDataReceived(const CanData& can_data)
{
    // Forward data to the processing thread
    m_rx_mailbox.post(can_data);
}

/** \brief Rx thread */
void CanPipeline::rxThread()
{
    CanData can_data;

    // Thread loop, wait for CAN data
    while (m_rx_mailbox.wait(can_data))
    {
        // Dispatch data
        m_suspender.canDataReceived(can_data);
    }
}
