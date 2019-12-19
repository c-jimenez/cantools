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

#ifndef CANADAPTERENUMERATORAPI_H
#define CANADAPTERENUMERATORAPI_H

#include "CanAdapterControllerApi.h"


/** \brief CAN controller information */
typedef struct _CanControllerInformation
{
    /** \brief Controller name */
    const char* name;
    /** \brief Controller description */
    const char* description;
} CanControllerInformation;


/** \brief Get the list of available controllers */
typedef bool (DYN_LINK_API *fp_can_adapter_enumerator_enumerate_controllers_t)(void* enumerator, CanControllerInformation** controllers);

/** \brief Instanciate a CAN controller from its enumerate name */
typedef CanAdapterController* (DYN_LINK_API *fp_can_adapter_enumerator_create_controller_t)(void* enumerator, const char* controller_name);

/** \brief Retrieve the last error message */
typedef const char* (DYN_LINK_API *fp_can_adapter_enumerator_get_error_msg_t)(void* enumerator);


/** \brief CAN adapter enumerator object */
typedef struct _CanAdapterEnumerator
{
    fp_can_adapter_enumerator_enumerate_controllers_t enumerate_controllers;
    fp_can_adapter_enumerator_create_controller_t create_controller;
    fp_can_adapter_enumerator_get_error_msg_t get_error_msg;
    void* enumerator;
} CanAdapterEnumerator;

#endif // CANADAPTERENUMERATORAPI_H
