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

#ifndef IPLUGIN_H
#define IPLUGIN_H

#include "NoCopy.h"

#include <string>
#include <vector>

typedef struct _PluginParameter PluginParameter;

/** \brief Interface for all the plugins implementations */
class IPlugin : public INoCopy
{
    public:

        /** \brief Destructor */
        virtual ~IPlugin() {}


        /** \brief Get the plugin's type */
        virtual const char* getType() const = 0;

        /** \brief Get the plugin's name */
        virtual const char* getName() const = 0;

        /** \brief Get the plugin's version */
        virtual const char* getVersion() const = 0;      

        /** \brief Get the plugin's parameters */
        virtual const std::vector<const PluginParameter*>& getParameters() const = 0;
};


#endif // IPLUGIN_H
