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

#ifndef ICANMSGDATABASE_H
#define ICANMSGDATABASE_H

#include "CanMsgDesc.h"

#include <map>
#include <memory>

/** \brief Interface for all CAN message database implementations */
class ICanMsgDatabase
{
    public:

        /** \brief Destructor */
        virtual ~ICanMsgDatabase() {}


        /** \brief Get the list of CAN messages */
        virtual const std::map<uint32_t, std::shared_ptr<CanMsgDesc>>& getCanMsgs() const = 0;


};


#endif // ICANMSGDATABASE_H
