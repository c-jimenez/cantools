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

#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include "IPlugin.h"
#include "PluginFactory.h"
#include "NoCopy.h"

#include <map>
#include <vector>
#include <memory>
#include <functional>

/** \brief Utility class to load plugins */
class PluginLoader : public INoCopy
{
    public:

        /** \brief Constructor */
        PluginLoader(const std::map<std::string, std::unique_ptr<IPluginFactory>>& factories);
        /** \brief Destructor */
        virtual ~PluginLoader();


        /** \brief Load plugins from a specified path */
        bool load(const std::string& search_path, std::vector<IPlugin*>& plugins);

        /** \brief Load plugins from specified paths */
        bool load(const std::vector<std::string>& search_paths, std::vector<IPlugin*>& plugins);

        /** \brief Load plugins from a specified path */
        bool load(const std::string& search_path, std::vector<IPlugin*>& plugins, std::function<bool(IPlugin*)> filter_func);

        /** \brief Load plugins from specified paths */
        bool load(const std::vector<std::string>& search_paths, std::vector<IPlugin*>& plugins, std::function<bool(IPlugin*)> filter_func);


    private:

        /** \brief Type */
        const std::map<std::string, std::unique_ptr<IPluginFactory>>& m_factories;
        
};


#endif // PLUGINLOADER_H
