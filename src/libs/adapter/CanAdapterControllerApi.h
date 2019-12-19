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

#ifndef CANADAPTERCONTROLLERAPI_H
#define CANADAPTERCONTROLLERAPI_H

#include "PluginHelper.h"

#include "CanBitrate.h"
#include "CanMode.h"
#include "CanData.h"


/** \brief CAN adapter controller capabilities */
typedef struct _CanAdapterControllerCapabilities
{
    /** \brief Maximum bitrate supported */
    uint16_t max_bitrate;
    /** \brief Indicates if the controller supports extended mode */
    bool extended;
    /** \brief Indicates if the controller supports bus load measurement */
    bool bus_load;
    /** \brief Indicates if the controller supports no acknowledge mode */
    bool no_ack;
} CanAdapterControllerCapabilities;


/** \brief Callback when CAN data has been received on the controller */
typedef void (DYN_LINK_API *fp_can_adapter_controller_can_data_received_t)(void* parameter, const CanData* can_data);

/** \brief CAN adapter controller callbacks */
typedef struct _CanAdapterControllerCallbacks
{
    fp_can_adapter_controller_can_data_received_t data_received;
    void* parameter;
} CanAdapterControllerCallbacks;


/** \brief Open the controller */
typedef bool (DYN_LINK_API *fp_can_adapter_controller_open_t)(void* controller);

/** \brief Close the controller */
typedef bool (DYN_LINK_API *fp_can_adapter_controller_close_t)(void* controller);

/** \brief Configure the controller */
typedef bool (DYN_LINK_API *fp_can_adapter_controller_configure_t)(void* controller, const CanBitrate bitrate, const uint8_t mode);

/** \brief Get the controller's capababilities */
typedef bool (DYN_LINK_API *fp_can_adapter_controller_get_capabilities_t)(void* controller, CanAdapterControllerCapabilities* caps);

/** \brief Start the controller */
typedef bool (DYN_LINK_API *fp_can_adapter_controller_start_t)(void* controller, const CanAdapterControllerCallbacks* callbacks);

/** \brief Stop the controller */
typedef bool (DYN_LINK_API *fp_can_adapter_controller_stop_t)(void* controller);

/** \brief Send a message through the controller */
typedef bool(DYN_LINK_API *fp_can_adapter_controller_send_t)(void* controller, const CanMsg* can_msg);

/** \brief Retrieve the last error message */
typedef const char* (DYN_LINK_API *fp_can_adapter_controller_get_error_msg_t)(void* controller);


/** \brief CAN adapter controller object */
typedef struct _CanAdapterController
{
    fp_can_adapter_controller_open_t open;
    fp_can_adapter_controller_close_t close;
    fp_can_adapter_controller_configure_t configure;
    fp_can_adapter_controller_get_capabilities_t get_capabilities;
    fp_can_adapter_controller_start_t start;
    fp_can_adapter_controller_stop_t stop;
    fp_can_adapter_controller_send_t send;
    fp_can_adapter_controller_get_error_msg_t get_error_msg;
    void* controller;
} CanAdapterController;

#endif // CANADAPTERCONTROLLERAPI_H
