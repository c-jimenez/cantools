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

#ifndef IXXATCANENUMERATOR_H
#define IXXATCANENUMERATOR_H

#include "IxxatCanAdapterEnumerator.h"
#include "NoCopy.h"

#include <map>
#include <string>
#include <vector>
#include <cstdint>

/** \brief CAN controller enumerator for IXXAT CAN interfaces */
class IxxatCanEnumerator : public INoCopy
{
    public:

        /** \brief Constructor */
        IxxatCanEnumerator();

        /** \brief Destructor */
        virtual ~IxxatCanEnumerator();


        /** \brief Get the list of available Controllers */
        bool enumerateControllers(CanControllerInformation** controllers);

        /** \brief Instanciate a CAN controller from its enumerate name */
        CanAdapterController* createController(const std::string& controller_name);

        /** \brief Instanciate a CAN controller from its hardware number and can line */
        CanAdapterController* createController(const std::string& hwnumber, const uint16_t line);


        /** \brief Retrieve the last error message */
        const char* getErrorMsg() const { return m_last_error.c_str(); }


    private:

        /** \brief Last error message */
        std::string m_last_error;

        /** \brief Devices list */
        std::map < std::string, std::pair<int64_t, uint16_t> > m_controllers;

        /** \brief User devices list */
        std::vector<CanControllerInformation> m_user_controllers;


        /** \brief Release memory allocated by the user device list */
        void releaseUserDeviceList();

        /** \brief Add a device to the user device list */
        void addToUserDeviceList(const char* name, const char* description);
};


#endif // IXXATCANENUMERATOR_H
