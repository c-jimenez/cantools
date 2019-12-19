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

#include "CanAdapterPluginFactory.h"
#include "PluginFactory.h"
#include "PluginLoader.h"
#include "VectorHelper.h"

#include <functional>

using namespace std;

/** \brief Filter object to load only the requested adapters */
class CanAdapterPluginFilter
{
    public:

        /** \brief Constructor */
        CanAdapterPluginFilter(const string& adapter_name)
        : m_adapter_names()
        {
            m_adapter_names.push_back(adapter_name);
        }

        /** \brief Constructor */
        CanAdapterPluginFilter(const vector<string>& adapter_names)
        : m_adapter_names(adapter_names)
        {}

        /** \brief Filter function */
        bool filter(IPlugin* plugin) const
        {
            return (find<string>(m_adapter_names, plugin->getName()) != m_adapter_names.cend());
        }

    private:

        /** \brief Names of the selected adapters */
        vector<string> m_adapter_names;
};


/** \brief Load the requested CAN adapter plugin */
CanAdapterPlugin* CanAdapterPluginFactory::load(const std::string& plugin_path, const std::string& adapter_name)
{
    CanAdapterPlugin* adapter_plugin = nullptr;

    // Prepare filter
    CanAdapterPluginFilter plugin_filter(adapter_name);
    auto filter_func = std::bind(&CanAdapterPluginFilter::filter, &plugin_filter, std::placeholders::_1);

    // Prepare factory
    map<string, unique_ptr<IPluginFactory>> factories;
    factories[CAN_ADAPTER_PLUGIN_TYPE] = unique_ptr<IPluginFactory>(new GenericPluginFactory<CanAdapterPlugin>());

    // Load plugin
    PluginLoader loader(factories);
    vector<IPlugin*> plugins;
    if (loader.load(plugin_path, plugins, filter_func))
    {
        if (plugins.size() == 1u)
        {
            adapter_plugin = dynamic_cast<CanAdapterPlugin*>(plugins[0]);
        }
    }

    return adapter_plugin;
}

/** \brief Load the requested CAN adapter plugins */
bool CanAdapterPluginFactory::load(const std::string& plugin_path, const std::vector<std::string>& adapter_names, std::vector<CanAdapterPlugin*>& plugins)
{
    bool ret = false;

    // Prepare filter
    CanAdapterPluginFilter plugin_filter(adapter_names);
    auto filter_func = std::bind(&CanAdapterPluginFilter::filter, &plugin_filter, std::placeholders::_1);

    // Prepare factory
    map<string, unique_ptr<IPluginFactory>> factories;
    factories[CAN_ADAPTER_PLUGIN_TYPE] = unique_ptr<IPluginFactory>(new GenericPluginFactory<CanAdapterPlugin>());

    // Load plugin
    PluginLoader loader(factories);
    vector<IPlugin*> iplugins;
    plugins.clear();
    ret = loader.load(plugin_path, iplugins, filter_func);
    if (ret)
    {
        for (size_t i = 0; i < iplugins.size(); i++)
        {
            plugins.push_back(dynamic_cast<CanAdapterPlugin*>(iplugins[i]));
        }
    }

    return ret;
}

/** \brief Load all the available CAN adapter plugins */
bool CanAdapterPluginFactory::load(const std::string& plugin_path, std::vector<CanAdapterPlugin*>& plugins)
{
    bool ret = false;

    // Prepare factory
    map<string, unique_ptr<IPluginFactory>> factories;
    factories[CAN_ADAPTER_PLUGIN_TYPE] = unique_ptr<IPluginFactory>(new GenericPluginFactory<CanAdapterPlugin>());

    // Load plugin
    PluginLoader loader(factories);
    vector<IPlugin*> iplugins;
    plugins.clear();
    ret = loader.load(plugin_path, iplugins);
    if (ret)
    {
        for (size_t i = 0; i < iplugins.size(); i++)
        {
            plugins.push_back(dynamic_cast<CanAdapterPlugin*>(iplugins[i]));
        }
    }

    return ret;
}