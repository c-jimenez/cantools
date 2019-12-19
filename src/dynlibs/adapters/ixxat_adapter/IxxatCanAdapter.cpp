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
#include "IxxatCanController.h"
#include "IxxatCanEnumerator.h"
#include "IxxatCanAdapterEnumerator.h"
#include "ParameterString.h"

#include <string>
using namespace std;


/** \brief Adapter's enumerator */
static CanAdapterEnumerator* ixxat_enumerator = nullptr;


/** \brief Get the plugin's type */
static const char* DYN_LINK_API CAN_ADAPTER_GetType(void)
{
    return CAN_ADAPTER_PLUGIN_TYPE;
}

/** \brief Get the plugin's name */
static const char* DYN_LINK_API CAN_ADAPTER_GetName(void)
{
    return "ixxat_adapter";
}

/** \brief Get the plugin's version */
static const char* DYN_LINK_API CAN_ADAPTER_GetVersion(void)
{
    return "1.0";
}

/** \brief Get the plugin's parameters */
static const PluginParameter* DYN_LINK_API CAN_ADAPTER_GetParams(void)
{
    static const PluginParameter plugin_params[] = 
    {
        {"hwnumber", "string", "Device unique HW number => HWXXXXXX. Default = first on enumeration list."},
        {"line", "int", "For adapter with multiple CAN interfaces only. Default = 0."},
        {nullptr, nullptr, nullptr}
    };

    return plugin_params;
}

/** \brief Get the adapter's enumerator */
static CanAdapterEnumerator* DYN_LINK_API CAN_ADAPTER_GetEnumerator(void)
{
    if (ixxat_enumerator == nullptr)
    {
        ixxat_enumerator = new CanAdapterEnumerator();
        ixxat_enumerator->enumerate_controllers = &CanAdapterEnumerator_EnumerateControllers;
        ixxat_enumerator->create_controller = &CanAdapterEnumerator_CreateController;
        ixxat_enumerator->get_error_msg = &CanAdapterEnumerator_GetErrorMsg;
        ixxat_enumerator->enumerator = new IxxatCanEnumerator();
    }

    return ixxat_enumerator;
}

/** \brief Release the adapter's enumerator */
void DYN_LINK_API CAN_ADAPTER_ReleaseEnumerator(void)
{
    if (ixxat_enumerator != nullptr)
    {
        IxxatCanEnumerator* enumerator = reinterpret_cast<IxxatCanEnumerator*>(ixxat_enumerator->enumerator);
        delete enumerator;
        delete ixxat_enumerator;
        ixxat_enumerator = nullptr;
    }
}

/** \brief Create a can controller from the specified parameters */
static CanAdapterController* DYN_LINK_API CAN_ADAPTER_CreateController(const char* parameters)
{
    ParameterString param_str(parameters);
    const string& hwnumber = param_str.getParam("hwnumber", "");
    uint16_t line = static_cast<uint16_t>(param_str.getParam("line", 0ull));

    CAN_ADAPTER_GetEnumerator();
    IxxatCanEnumerator* enumerator = reinterpret_cast<IxxatCanEnumerator*>(ixxat_enumerator->enumerator);
    return enumerator->createController(hwnumber, line);
}

/** \brief Release a can controller */
static void DYN_LINK_API CAN_ADAPTER_ReleaseController(CanAdapterController* controller)
{
    IxxatCanController* ixxat_controller = reinterpret_cast<IxxatCanController*>(controller->controller);
    delete ixxat_controller;
    delete controller;
}


/** \brief CAN adapter's functions */
static const CanAdapterPluginFuncs can_adapter_funcs = {
                                                        &CAN_ADAPTER_GetEnumerator,
                                                        &CAN_ADAPTER_ReleaseEnumerator,
                                                        &CAN_ADAPTER_CreateController,
                                                        &CAN_ADAPTER_ReleaseController
                                                       };

/** \brief Plugin's generic functions */
static const PluginGenericFuncs plugin_generic_funcs = {
                                                        &CAN_ADAPTER_GetType,
                                                        &CAN_ADAPTER_GetName,
                                                        &CAN_ADAPTER_GetVersion,
                                                        &CAN_ADAPTER_GetParams,
                                                        &can_adapter_funcs
                                                       };


/** \brief Get the plugin's functions */
DYN_LINK_EXPORT const PluginGenericFuncs* DYN_LINK_API PLUGIN_GetFuncs()
{
    return &plugin_generic_funcs;
}
