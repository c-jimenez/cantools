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

#include "CanAdapterPluginApi.h"

#include "UdpCanController.h"


/** \brief Open the controller */
bool DYN_LINK_API CanAdapterController_Open(void* controller)
{
    UdpCanController* udp_controller = reinterpret_cast<UdpCanController*>(controller);
    return udp_controller->open();
}

/** \brief Close the controller */
bool DYN_LINK_API CanAdapterController_Close(void* controller)
{
    UdpCanController* udp_controller = reinterpret_cast<UdpCanController*>(controller);
    return udp_controller->close();
}

/** \brief Configure the controller */
bool DYN_LINK_API CanAdapterController_Configure(void* controller, const CanBitrate bitrate, const uint8_t mode)
{
    UdpCanController* udp_controller = reinterpret_cast<UdpCanController*>(controller);
    return udp_controller->configure(bitrate, mode);
}

/** \brief Get the controller's capababilities */
bool DYN_LINK_API CanAdapterController_GetCapabilities(void* controller, CanAdapterControllerCapabilities* caps)
{
    UdpCanController* udp_controller = reinterpret_cast<UdpCanController*>(controller);
    return udp_controller->getCapabilities(caps);
}

/** \brief Start the controller */
bool DYN_LINK_API CanAdapterController_Start(void* controller, const CanAdapterControllerCallbacks* callbacks)
{
    UdpCanController* udp_controller = reinterpret_cast<UdpCanController*>(controller);
    return udp_controller->start(callbacks);
}

/** \brief Stop the controller */
bool DYN_LINK_API CanAdapterController_Stop(void* controller)
{
    UdpCanController* udp_controller = reinterpret_cast<UdpCanController*>(controller);
    return udp_controller->stop();
}

/** \brief Send a message through the controller */
bool DYN_LINK_API CanAdapterController_Send(void* controller, const CanMsg* can_msg)
{
    UdpCanController* udp_controller = reinterpret_cast<UdpCanController*>(controller);
    return udp_controller->send(can_msg);
}

/** \brief Retrieve the last error message */
const char* DYN_LINK_API CanAdapterController_GetErrorMsg(void* controller)
{
    UdpCanController* udp_controller = reinterpret_cast<UdpCanController*>(controller);
    return udp_controller->getErrorMsg();
}

