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

#include "Plugin.h"

using namespace std;

/** \brief Constructor */
Plugin::Plugin(DynLibLoader* loader, const PluginGenericFuncs* plugin_funcs)
: m_loader(loader)
, m_plugin_funcs(plugin_funcs)
{
    const PluginParameter* params = plugin_funcs->get_params();
    if (params != nullptr)
    {
        while (params->name != nullptr)
        {
            m_parameters.push_back(params);
            params++;
        }
    }
}

/** \brief Destructor */
Plugin::~Plugin()
{
    m_loader->unload();
    delete m_loader;
}
