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

#include "IxxatCanEnumerator.h"
#include "IxxatCanController.h"
#include "IxxatCanAdapterController.h"
#include "IxxatUtils.h"

#include <sstream>

using namespace std;

/** \brief Constructor */
IxxatCanEnumerator::IxxatCanEnumerator()
: m_last_error("")
, m_controllers()
, m_user_controllers()
{}

/** \brief Destructor */
IxxatCanEnumerator::~IxxatCanEnumerator()
{
    releaseUserDeviceList();
}


/** \brief Get the list of available Controllers */
bool IxxatCanEnumerator::enumerateControllers(CanControllerInformation** controllers)
{
    HRESULT res = VCI_E_INVALIDARG;

    if (controllers != nullptr)
    {
        // Reset list
        releaseUserDeviceList();
        m_controllers.clear();

        // Open device list
        HANDLE vci_handle;
        res = vciEnumDeviceOpen(&vci_handle);
        if (res != VCI_OK)
        {
            IxxatUtils::errorToString(res, m_last_error);
        }
        else
        {
            // Enumerate devices
            VCIDEVICEINFO dev_info = { 0 };
            while ((res = vciEnumDeviceNext(vci_handle, &dev_info)) == VCI_OK)
            {
                // Open device
                HANDLE hdevice;
                res = vciDeviceOpen(dev_info.VciObjectId, &hdevice);
                if (res == VCI_OK)
                {
                    // Get device capabilities
                    VCIDEVICECAPS dev_caps = { 0 };
                    res = vciDeviceGetCaps(hdevice, &dev_caps);
                    if (res == VCI_OK)
                    {
                        // For each controller inside the device
                        for (uint16_t i = 0; i < dev_caps.BusCtrlCount; i++)
                        {
                            // Check if the device is a CAN controller
                            if (VCI_BUS_TYPE(dev_caps.BusCtrlTypes[i]) == VCI_BUS_CAN)
                            {
                                // Add the device to the list
                                string device_name;
                                IxxatUtils::buildDeviceName(dev_info, i, device_name);
    
                                addToUserDeviceList(device_name.c_str(), dev_info.Description);
                                m_controllers[device_name] = pair<int64_t, uint16_t>(dev_info.VciObjectId.AsInt64, i);
                            }
                        }
                    }

                    // Close device
                    vciDeviceClose(hdevice);
                }
            }
            if ((res != VCI_OK) && (res != VCI_E_NO_MORE_ITEMS))
            {
                IxxatUtils::errorToString(res, m_last_error);
            }
            else
            {
                res = VCI_OK;

                // Add empty entry and return raw list
                addToUserDeviceList(nullptr, nullptr);
                (*controllers) = &m_user_controllers[0];
            }

            // Close device list
            vciEnumDeviceClose(vci_handle);
        }
    }
    else
    {
        IxxatUtils::errorToString(res, m_last_error);
    }

    return (res == VCI_OK);
}

/** \brief Instanciate a CAN controller */
CanAdapterController* IxxatCanEnumerator::createController(const std::string& controller_name)
{
    CanAdapterController* controller = nullptr;

    // Look for the controller in the device's list
    auto iter = m_controllers.find(controller_name);
    if (iter != m_controllers.end())
    {
        // Instanciate controller
        VCIID vciid;
        vciid.AsInt64 = iter->second.first;

        controller = new CanAdapterController();
        controller->open = &CanAdapterController_Open;
        controller->close = &CanAdapterController_Close;
        controller->configure = &CanAdapterController_Configure;
        controller->get_capabilities = &CanAdapterController_GetCapabilities;
        controller->start = &CanAdapterController_Start;
        controller->stop = &CanAdapterController_Stop;
        controller->send = &CanAdapterController_Send;
        controller->get_error_msg = &CanAdapterController_GetErrorMsg;
        controller->controller = new IxxatCanController(vciid, iter->second.second);
    }

    return controller;
}

/** \brief Instanciate a CAN controller from its hardware number and can line */
CanAdapterController* IxxatCanEnumerator::createController(const std::string& hwnumber, const uint16_t line)
{
    // Enumerate devices
    CanControllerInformation* controllers = nullptr;
    enumerateControllers(&controllers);

    // Instanciate controller
    stringstream controller_name;
    controller_name << hwnumber << "-" << line;
    return createController(controller_name.str());
}

/** \brief Release memory allocated by the user device list */
void IxxatCanEnumerator::releaseUserDeviceList()
{
    for (size_t i = 0; i < m_user_controllers.size(); i++)
    {
        delete[] m_user_controllers[i].name;
        delete[] m_user_controllers[i].description;
    }
    m_user_controllers.clear();
}

/** \brief Add a device to the user device list */
void IxxatCanEnumerator::addToUserDeviceList(const char* name, const char* description)
{
    CanControllerInformation controller = { 0 };
    if (name != nullptr)
    {
        char* value = new char[strlen(name) + 1u];
        strcpy(value, name);
        controller.name = value;
    }
    if (description != nullptr)
    {
        char* value = new char[strlen(description) + 1u];
        strcpy(value, description);
        controller.description = value;
    }
    m_user_controllers.push_back(controller);
}
