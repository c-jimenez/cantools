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

#ifndef CANADAPTERPLUGIN_H
#define CANADAPTERPLUGIN_H

#include "Plugin.h"
#include "CanAdapterPluginApi.h"
#include "CanControllerEnumerator.h"

/** \brief CAN adapter plugin */
class CanAdapterPlugin : public Plugin
{
    public:

        /** \brief Constructor */
        CanAdapterPlugin(DynLibLoader* loader, const PluginGenericFuncs* plugin_funcs);
        /** \brief Destructor */
        virtual ~CanAdapterPlugin();



        /** \brief Get the adapter's enumerator */
        CanControllerEnumerator* getEnumerator(void);

        /** \brief Create a can controller from the specified parameters */
        CanController* createController(const char* parameters);


    private:

        /** \brief Adapter plugin functions */
        const CanAdapterPluginFuncs* m_plugin_funcs;

        /** \brief Adapter's enumerator */
        CanControllerEnumerator* m_enumerator;

};


#endif // CANADAPTERPLUGIN_H
