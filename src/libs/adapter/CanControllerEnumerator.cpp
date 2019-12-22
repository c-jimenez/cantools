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

#include "CanControllerEnumerator.h"


/** \brief Constructor */
CanControllerEnumerator::CanControllerEnumerator(const CanAdapterEnumerator* enumerator, const CanAdapterPluginFuncs* plugin_funcs)
: m_enumerator(enumerator)
, m_plugin_funcs(plugin_funcs)
{}

/** \brief Destructor */
CanControllerEnumerator::~CanControllerEnumerator()
{
    m_plugin_funcs->release_enumerator();
}

/** \brief Get the list of available Controllers */
bool CanControllerEnumerator::enumerateControllers(std::vector<CanControllerInformation*>& controllers)
{
    CanControllerInformation* controller_infos = nullptr;
    bool ret = m_enumerator->enumerate_controllers(m_enumerator->enumerator, &controller_infos);
    if (ret)
    {
        controllers.clear();
        while (controller_infos->name != nullptr)
        {
            controllers.push_back(controller_infos);
            controller_infos++;
        }
    }
    return ret;
}

/** \brief Instanciate a CAN controller from its enumerate name */
CanController* CanControllerEnumerator::createController(const std::string& controller_name)
{
    CanController* controller = nullptr;
    CanAdapterController* can_controller = m_enumerator->create_controller(m_enumerator->enumerator, controller_name.c_str());
    if (can_controller != nullptr)
    {
        controller = new CanController(can_controller, m_plugin_funcs);
    }
    return controller;
}
