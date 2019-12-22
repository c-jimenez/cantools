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

#ifndef MATHPLUGIN_H
#define MATHPLUGIN_H

#include "Plugin.h"
#include "MathPluginApi.h"


class FunctionCallExpression;

/** \brief Math plugin */
class MathPlugin : public Plugin
{
    public:

        /** \brief Constructor */
        MathPlugin(DynLibLoader* loader, const PluginGenericFuncs* plugin_funcs);
        /** \brief Destructor */
        virtual ~MathPlugin();



        /** \brief Get the math functions */
        void getMathFunctions(std::vector<std::pair<std::string, FunctionCallExpression*>>& functions);



    private:

        /** \brief Math plugin functions */
        const MathPluginFuncs* m_plugin_funcs;

};


#endif // CANADAPTERPLUGIN_H
