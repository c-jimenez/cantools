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

#include "CanController.h"
#include "ICanControllerListener.h"
#include "osal.h"


/** \brief Constructor */
CanController::CanController(CanAdapterController* controller, const CanAdapterPluginFuncs* plugin_funcs)
: m_controller(controller)
, m_plugin_funcs(plugin_funcs)
, m_listener(nullptr)
{}

/** \brief Destructor */
CanController::~CanController()
{
    m_plugin_funcs->release_controller(m_controller);
}


/** \brief Start the controller */
bool CanController::start(ICanControllerListener& listener)
{
    CanAdapterControllerCallbacks callbacks;
    callbacks.data_received = &CanController::canDataReceivedCallback;
    callbacks.parameter = this;
    m_listener = &listener;
    return m_controller->start(m_controller->controller, &callbacks);
}

/** \brief Send a message through the controller */
bool CanController::send(const CanMsg& can_msg) 
{
    const bool ret = m_controller->send(m_controller->controller, &can_msg);
    if (ret)
    {
        CanData can_data = { 0 };
        can_data.header.type = CAN_DATA_TX_MSG;
        OSAL_GetTimestamp(&can_data.header.timestamp_sec, &can_data.header.timestamp_nsec);
        can_data.msg = can_msg;
        m_listener->canDataReceived(can_data);
    }

    return ret;
}

/** \brief Callback when CAN data has been received on the controller */
void DYN_LINK_API CanController::canDataReceivedCallback(void* parameter, const CanData* can_data)
{
    CanController* controller = reinterpret_cast<CanController*>(parameter);
    controller->m_listener->canDataReceived(*can_data);
}
