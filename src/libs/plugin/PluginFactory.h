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

#ifndef PLUGINFACTORY_H
#define PLUGINFACTORY_H


#include "IPlugin.h"
#include "DynLibLoader.h"
#include "PluginApi.h"


/** \brief Interface for all the plugin factories implementations */
class IPluginFactory
{
    public:

        /** \brief Destructor */
        virtual ~IPluginFactory() {}

        /** \brief Instanciate a new plugin object */
        virtual IPlugin* create(DynLibLoader* loader, const PluginGenericFuncs* plugin_funcs) const = 0;
};

/** \brief Generic plugin factory implementation */
template <typename PluginType>
class GenericPluginFactory : public IPluginFactory
{
    public:

        /** \brief Constructor */
        GenericPluginFactory()
        {}

        /** \brief Destructor */
        virtual ~GenericPluginFactory() 
        {}

        /** \brief Instanciate a new plugin object */
        virtual IPlugin* create(DynLibLoader* loader, const PluginGenericFuncs* plugin_funcs) const override
        {
            return new PluginType(loader, plugin_funcs);
        }
};

#endif // PLUGINFACTORY_H
