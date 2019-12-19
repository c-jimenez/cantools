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

#ifndef MATHPLUGINFACTORY_H
#define MATHPLUGINFACTORY_H

#include "MathPlugin.h"

/** \brief Math plugin factory */
class MathPluginFactory
{
    public:

        /** \brief Load the requested math plugin */
        static MathPlugin* load(const std::string& plugin_path, const std::string& name);

        /** \brief Load the requested math plugins */
        static bool load(const std::string& plugin_path, const std::vector<std::string>& names, std::vector<MathPlugin*>& plugins);

        /** \brief Load all the available math plugins */
        static bool load(const std::string& plugin_path, std::vector<MathPlugin*>& plugins);
};


#endif // MATHPLUGINFACTORY_H
