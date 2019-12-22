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

#ifndef PLUGIN_H
#define PLUGIN_H

#include "IPlugin.h"
#include "PluginApi.h"
#include "DynLibLoader.h"

/** \brief Base class for the plugins */
class Plugin : public IPlugin
{
    public:

        /** \brief Constructor */
        Plugin(DynLibLoader* loader, const PluginGenericFuncs* plugin_funcs);
        /** \brief Destructor */
        virtual ~Plugin();


        /** \brief Get the plugin's type */
        virtual const char* getType() const override { return m_plugin_funcs->get_type(); }

        /** \brief Get the plugin's name */
        virtual const char* getName() const override { return m_plugin_funcs->get_name(); }

        /** \brief Get the plugin's version */
        virtual const char* getVersion() const override { return m_plugin_funcs->get_version(); }

        /** \brief Get the plugin's parameters */
        virtual const std::vector<const PluginParameter*>& getParameters() const override { return m_parameters; }


    private:

        /** \brief Dynamic library loader */
        DynLibLoader* m_loader;

        /** \brief Generic plugin functions */
        const PluginGenericFuncs* m_plugin_funcs;

        /** \brief Plugin's parameters */
        std::vector<const PluginParameter*> m_parameters;

};


#endif // PLUGIN_H
