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
#include "osal.h"

#include <stdlib.h>
#include <string.h>


/** \brief Loopback controller instance data */
typedef struct _loopback_controller_t
{
    /** \brief Indicate if the controller is opened */
    bool opened;
    /** \brief Indicate if the controller is started */
    bool started;
    /** \brief Bitrate */
    CanBitrate bitrate;
    /** \brief Mode */
    uint8_t mode;
    /** \brief Callbacks */
    CanAdapterControllerCallbacks callbacks;
    /** \brief Last error message */
    const char* last_error_message;
} loopback_controller_t;



/** \brief Allocate a controller */
void* CanAdapterController_Allocate(void)
{
    return calloc(1u, sizeof(loopback_controller_t));
}

/** \brief Release a controller */
void CanAdapterController_Release(void* controller)
{
    free(controller);
}


/** \brief Open the controller */
bool DYN_LINK_API CanAdapterController_Open(void* controller)
{
    bool ret = false;
    loopback_controller_t* loopback_controller = controller;
    
    if (OSAL_Init())
    {
        if (loopback_controller != NULL)
        {
            if (!loopback_controller->opened)
            {
                loopback_controller->opened = true;
                ret = true;
            }
            else
            {
                loopback_controller->last_error_message = "Controller already opened";
            }
        }
    }

    return ret;
}

/** \brief Close the controller */
bool DYN_LINK_API CanAdapterController_Close(void* controller)
{
    bool ret = false;
    loopback_controller_t* loopback_controller = controller;

    if (loopback_controller != NULL)
    {
        if (loopback_controller->opened)
        {
            loopback_controller->opened = false;
            loopback_controller->started = false;
            ret = true;
        }
        else
        {
            loopback_controller->last_error_message = "Controller not opened";
        }
    }

    return ret;
}

/** \brief Configure the controller */
bool DYN_LINK_API CanAdapterController_Configure(void* controller, const CanBitrate bitrate, const uint8_t mode)
{
    bool ret = false;
    loopback_controller_t* loopback_controller = controller;

    if (loopback_controller != NULL)
    {
        if (loopback_controller->opened)
        {
            loopback_controller->bitrate = bitrate;
            loopback_controller->mode = mode;
            ret = true;
        }
        else
        {
            loopback_controller->last_error_message = "Controller not opened";
        }
    }

    return ret;
}

/** \brief Get the controller's capababilities */
bool DYN_LINK_API CanAdapterController_GetCapabilities(void* controller, CanAdapterControllerCapabilities* caps)
{
    bool ret = false;
    loopback_controller_t* loopback_controller = controller;

    if ((loopback_controller != NULL) && (caps != NULL))
    {
        if (loopback_controller->opened)
        {
            caps->max_bitrate = CANBR_1000K;
            caps->extended = true;
            caps->bus_load = false;
            caps->no_ack = false;
            ret = true;
        }
        else
        {
            loopback_controller->last_error_message = "Controller not opened";
        }
    }

    return ret;
}

/** \brief Start the controller */
bool DYN_LINK_API CanAdapterController_Start(void* controller, const CanAdapterControllerCallbacks* callbacks)
{
    bool ret = false;
    loopback_controller_t* loopback_controller = controller;

    if ((loopback_controller != NULL) && (callbacks != NULL))
    {
        if (loopback_controller->opened)
        {
            if (!loopback_controller->started)
            {
                loopback_controller->started = true;
                memcpy(&loopback_controller->callbacks, callbacks, sizeof(CanAdapterControllerCallbacks));
                ret = true;
            }
            else
            {
                loopback_controller->last_error_message = "Controller not started";
            }
        }
        else
        {
            loopback_controller->last_error_message = "Controller not opened";
        }
    }

    return ret;
}

/** \brief Stop the controller */
bool DYN_LINK_API CanAdapterController_Stop(void* controller)
{
    bool ret = false;
    loopback_controller_t* loopback_controller = controller;

    if (loopback_controller != NULL)
    {
        if (loopback_controller->opened)
        {
            if (loopback_controller->started)
            {
                loopback_controller->started = false;
                ret = true;
            }
            else
            {
                loopback_controller->last_error_message = "Controller not started";
            }
        }
        else
        {
            loopback_controller->last_error_message = "Controller not opened";
        }
    }

    return ret;
}

/** \brief Send a message through the controller */
bool DYN_LINK_API CanAdapterController_Send(void* controller, const CanMsg* can_msg)
{
    bool ret = false;
    loopback_controller_t* loopback_controller = controller;

    if (loopback_controller != NULL)
    {
        if (loopback_controller->opened)
        {
            if (loopback_controller->started)
            {
                if (can_msg->extended && ((loopback_controller->mode & CANMODE_EXT) == 0))
                {
                    loopback_controller->last_error_message = "Extended mode not supported";
                }
                else
                {
                    CanData can_data = { 0 };
                    can_data.header.type = CAN_DATA_RX_MSG;
                    can_data.msg = (*can_msg);
                    OSAL_GetTimestamp(&can_data.header.timestamp_sec, &can_data.header.timestamp_nsec);
                    loopback_controller->callbacks.data_received(loopback_controller->callbacks.parameter, &can_data);
                    ret = true;
                }
            }
            else
            {
                loopback_controller->last_error_message = "Controller not started";
            }
        }
        else
        {
            loopback_controller->last_error_message = "Controller not opened";
        }
    }

    return ret;
}

/** \brief Retrieve the last error message */
const char* DYN_LINK_API CanAdapterController_GetErrorMsg(void* controller)
{
    loopback_controller_t* loopback_controller = controller;
    if (loopback_controller != NULL)
    {
        return loopback_controller->last_error_message;
    }
    else
    {
        return "Invalid parameter";
    }
}
