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

#ifndef ICANCONTROLLERLISTENER_H
#define ICANCONTROLLERLISTENER_H

#include "CanData.h"

/** \brief Interface for CAN controller listeners implementations */
class ICanControllerListener
{
    public:

        /** \brief Destructor */
        virtual ~ICanControllerListener() {}


        /** \brief Called when CAN data has been received on the controller */
        virtual void canDataReceived(const CanData& can_data) = 0;
};


#endif // ICANCONTROLLERLISTENER_H
