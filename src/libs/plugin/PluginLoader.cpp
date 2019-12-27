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

#include "PluginLoader.h"
#include "FileSystemBrowser.h"

using namespace std;


/** \brief Constructor */
PluginLoader::PluginLoader(const std::map<std::string, std::unique_ptr<IPluginFactory>>& factories)
: m_factories(factories)
{}

/** \brief Destructor */
PluginLoader::~PluginLoader()
{}

/** \brief Load plugins from a specified path */
bool PluginLoader::load(const std::string& search_path, std::vector<IPlugin*>& plugins)
{
    auto accept_all_filter_func = [](IPlugin* plugin)->bool { return true; };
    return load(search_path, plugins, accept_all_filter_func);
}

/** \brief Load plugins from specified paths */
bool PluginLoader::load(const std::vector<std::string>& search_paths, std::vector<IPlugin*>& plugins)
{
    auto accept_all_filter_func = [](IPlugin* plugin)->bool { return true; };
    return load(search_paths, plugins, accept_all_filter_func);
}

/** \brief Load plugins from a specified path */
bool PluginLoader::load(const std::string& search_path, std::vector<IPlugin*>& plugins, const std::function<bool(IPlugin*)>& filter_func)
{
    vector<string> search_paths;
    search_paths.push_back(search_path);
    return load(search_paths, plugins, filter_func);
}

/** \brief Load plugins from specified paths */
bool PluginLoader::load(const std::vector<std::string>& search_paths, std::vector<IPlugin*>& plugins, const std::function<bool(IPlugin*)>& filter_func)
{
    bool ret = true;

    // Clear plugin list
    plugins.clear();

    // Look into all specified paths
    for (size_t i = 0; i < search_paths.size(); i++)
    {
        // List plugin files
        vector<string> plugin_files;
        ret = FileSystemBrowser::listFiles(search_paths[i], DynLibLoader::DYN_LIB_EXTENSION, plugin_files);
        if (ret)
        {
            // Load plugins
            for (size_t j = 0; j < plugin_files.size(); j++)
            {
                // Open library
                IPlugin* plugin = nullptr;
                DynLibLoader* loader = new DynLibLoader();
                ret = loader->load(plugin_files[j]);
                if (ret)
                {
                    // Look for the generic plugin functions
                    fp_plugin_get_funcs_t plugin_get_funcs = reinterpret_cast<fp_plugin_get_funcs_t>(loader->getFunctionPtr(FP_PLUGIN_GET_FUNCS_NAME));
                    if (plugin_get_funcs != nullptr)
                    {
                        // Get the plugin's type
                        const PluginGenericFuncs* plugin_funcs = plugin_get_funcs();
                        const char* plugin_type = plugin_funcs->get_type();

                        // Check if a factory exists for this type of plugin
                        auto iter = m_factories.find(plugin_type);
                        if (iter != m_factories.end())
                        {
                            // Instanciate plugin
                            const IPluginFactory* factory = (iter->second).get();
                            plugin = factory->create(loader, plugin_funcs);
                            if (plugin != nullptr)
                            {
                                // Apply filter
                                if (filter_func(plugin))
                                {
                                    plugins.push_back(plugin);
                                }
                                else
                                {
                                    delete plugin;
                                }
                            }
                        }
                    }
                }
                if (plugin == nullptr)
                {
                    delete loader;
                }
            }
        }
    }

    return ret;
}

