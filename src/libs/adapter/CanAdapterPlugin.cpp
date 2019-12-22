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

#include "CanAdapterPlugin.h"

using namespace std;

/** \brief Constructor */
CanAdapterPlugin::CanAdapterPlugin(DynLibLoader* loader, const PluginGenericFuncs* plugin_funcs)
: Plugin(loader, plugin_funcs)
, m_plugin_funcs(reinterpret_cast<const CanAdapterPluginFuncs*>(plugin_funcs->plugin_specific_funcs))
, m_enumerator(nullptr)
{}

/** \brief Destructor */
CanAdapterPlugin::~CanAdapterPlugin()
{}

/** \brief Get the adapter's enumerator */
CanControllerEnumerator* CanAdapterPlugin::getEnumerator(void)
{
    if (m_enumerator == nullptr)
    {
        const CanAdapterEnumerator* enumerator = m_plugin_funcs->get_enumerator();
        if (enumerator != nullptr)
        {
            m_enumerator = new CanControllerEnumerator(enumerator, m_plugin_funcs);
        }
    }

    return m_enumerator;
}

/** \brief Create a can controller from the specified parameters */
CanController* CanAdapterPlugin::createController(const char* parameters)
{
    CanController* controller = nullptr;
    CanAdapterController* can_controller = m_plugin_funcs->create_controller(parameters);
    if (can_controller != nullptr)
    {
        controller = new CanController(can_controller, m_plugin_funcs);
    }
    return controller;
}
