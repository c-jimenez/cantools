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

#include "MathPluginFactory.h"
#include "PluginFactory.h"
#include "PluginLoader.h"
#include "VectorHelper.h"

#include <functional>

using namespace std;

/** \brief Filter object to load only the requested plugins */
class MathPluginFilter
{
    public:

        /** \brief Constructor */
        MathPluginFilter(const string& name)
        : m_names()
        {
            m_names.push_back(name);
        }

        /** \brief Constructor */
        MathPluginFilter(const vector<string>& names)
        : m_names(names)
        {}

        /** \brief Filter function */
        bool filter(IPlugin* plugin) const
        {
            return (find<string>(m_names, plugin->getName()) != m_names.cend());
        }

    private:

        /** \brief Names of the selected adapters */
        vector<string> m_names;
};


/** \brief Load the requested math plugin */
MathPlugin* MathPluginFactory::load(const std::string& plugin_path, const std::string& name)
{
    MathPlugin* math_plugin = nullptr;

    // Prepare filter
    MathPluginFilter plugin_filter(name);
    auto filter_func = std::bind(&MathPluginFilter::filter, &plugin_filter, std::placeholders::_1);

    // Prepare factory
    map<string, unique_ptr<IPluginFactory>> factories;
    factories[MATH_PLUGIN_TYPE] = unique_ptr<IPluginFactory>(new GenericPluginFactory<MathPlugin>());

    // Load plugin
    PluginLoader loader(factories);
    vector<IPlugin*> plugins;
    if (loader.load(plugin_path, plugins, filter_func))
    {
        if (plugins.size() == 1u)
        {
            math_plugin = dynamic_cast<MathPlugin*>(plugins[0]);
        }
    }

    return math_plugin;
}

/** \brief Load the requested math plugins */
bool MathPluginFactory::load(const std::string& plugin_path, const std::vector<std::string>& names, std::vector<MathPlugin*>& plugins)
{
    bool ret = false;

    // Prepare filter
    MathPluginFilter plugin_filter(names);
    auto filter_func = std::bind(&MathPluginFilter::filter, &plugin_filter, std::placeholders::_1);

    // Prepare factory
    map<string, unique_ptr<IPluginFactory>> factories;
    factories[MATH_PLUGIN_TYPE] = unique_ptr<IPluginFactory>(new GenericPluginFactory<MathPlugin>());

    // Load plugin
    PluginLoader loader(factories);
    vector<IPlugin*> iplugins;
    plugins.clear();
    ret = loader.load(plugin_path, iplugins, filter_func);
    if (ret)
    {
        for (size_t i = 0; i < iplugins.size(); i++)
        {
            plugins.push_back(dynamic_cast<MathPlugin*>(iplugins[i]));
        }
    }

    return ret;
}

/** \brief Load all the available math plugins */
bool MathPluginFactory::load(const std::string& plugin_path, std::vector<MathPlugin*>& plugins)
{
    bool ret = false;

    // Prepare factory
    map<string, unique_ptr<IPluginFactory>> factories;
    factories[MATH_PLUGIN_TYPE] = unique_ptr<IPluginFactory>(new GenericPluginFactory<MathPlugin>());

    // Load plugin
    PluginLoader loader(factories);
    vector<IPlugin*> iplugins;
    plugins.clear();
    ret = loader.load(plugin_path, iplugins);
    if (ret)
    {
        for (size_t i = 0; i < iplugins.size(); i++)
        {
            plugins.push_back(dynamic_cast<MathPlugin*>(iplugins[i]));
        }
    }

    return ret;
}