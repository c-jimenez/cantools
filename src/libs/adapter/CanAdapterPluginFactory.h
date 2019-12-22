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

#ifndef CANADAPTERPLUGINFACTORY_H
#define CANADAPTERPLUGINFACTORY_H

#include "CanAdapterPlugin.h"

/** \brief CAN adapter plugin factory */
class CanAdapterPluginFactory
{
    public:

        /** \brief Load the requested CAN adapter plugin */
        static CanAdapterPlugin* load(const std::string& plugin_path, const std::string& adapter_name);

        /** \brief Load the requested CAN adapter plugins */
        static bool load(const std::string& plugin_path, const std::vector<std::string>& adapter_names, std::vector<CanAdapterPlugin*>& plugins);

        /** \brief Load all the available CAN adapter plugins */
        static bool load(const std::string& plugin_path, std::vector<CanAdapterPlugin*>& plugins);
};


#endif // CANADAPTERPLUGINFACTORY_H
