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

#ifndef CANCONTROLLERENUMERATOR_H
#define CANCONTROLLERENUMERATOR_H

#include "CanController.h"
#include "NoCopy.h"

#include <vector>
#include <string>

/** \brief Interface for CAN controller enumerator implementations */
class CanControllerEnumerator : public INoCopy
{
    public:

        /** \brief Constructor */
        CanControllerEnumerator(const CanAdapterEnumerator* enumerator, const CanAdapterPluginFuncs* plugin_funcs);

        /** \brief Destructor */
        virtual ~CanControllerEnumerator();


        /** \brief Get the list of available Controllers */
        bool enumerateControllers(std::vector<CanControllerInformation*>& controllers);

        /** \brief Instanciate a CAN controller from its enumerate name */
        CanController* createController(const std::string& controller_name);


        /** \brief Retrieve the last error message */
        const char* getErrorMsg() const { return m_enumerator->get_error_msg(m_enumerator->enumerator); }


    private:

        /** \brief CAN enumerator */
        const CanAdapterEnumerator* m_enumerator;

        /** \brief Plugin functions */
        const CanAdapterPluginFuncs* m_plugin_funcs;
};


#endif // CANCONTROLLERENUMERATOR_H
