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

#ifndef CANADAPTERPLUGIN_API_H
#define CANADAPTERPLUGIN_API_H

#include "PluginApi.h"
#include "CanAdapterControllerApi.h"
#include "CanAdapterEnumeratorApi.h"

/** \brief Adapter plugin type */
#define CAN_ADAPTER_PLUGIN_TYPE "can_adapter"


/** \brief Get the adapter's enumerator */
typedef CanAdapterEnumerator* (DYN_LINK_API *fp_can_adapter_get_enumerator_t)(void);

/** \brief Release the adapter's enumerator */
typedef void (DYN_LINK_API *fp_can_adapter_release_enumerator_t)(void);

/** \brief Create a can controller from the specified parameters */
typedef CanAdapterController* (DYN_LINK_API *fp_can_adapter_create_controller_t)(const char* parameters);

/** \brief Release a can controller */
typedef void (DYN_LINK_API *fp_can_adapter_release_controller_t)(CanAdapterController* controller);


/** \brief CAN adapter's plugin functions */
typedef struct _CanAdapterPluginFuncs
{
    fp_can_adapter_get_enumerator_t get_enumerator;
    fp_can_adapter_release_enumerator_t release_enumerator;
    fp_can_adapter_create_controller_t create_controller;
    fp_can_adapter_release_controller_t release_controller;
} CanAdapterPluginFuncs;


#endif // CANADAPTERPLUGIN_API_H
