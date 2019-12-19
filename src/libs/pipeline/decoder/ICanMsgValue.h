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

#ifndef ICANMSGVALUE_H
#define ICANMSGVALUE_H

#include "CanData.h"

#include <string>

class CanMsgDesc;

/** \brief Interface for CAN message values implementations */
class ICanMsgValue
{
    public:

        /** \brief Destructor */
        virtual ~ICanMsgValue() {}


        /** \brief Get the associated CAN message */
        virtual CanMsgDesc* getCanMsg() = 0;

        /** \brief Set the associated CAN message */
        virtual void setCanMsg(CanMsgDesc& can_msg) = 0;


        /** \brief Get the value's type */
        virtual const std::string& getType() const = 0;


        /** \brief Get the value's byte position inside the CAN message */
        virtual uint8_t getBytePosition() const = 0;

        /** \brief Set the value's byte position inside the CAN message */
        virtual bool setBytePosition(const uint8_t position) = 0;


        /** \brief Get the value's bit position inside its first byte into the CAN message */
        virtual uint8_t getBitPosition() const = 0;

        /** \brief Set the value's byte position inside its first byte into the CAN message */
        virtual bool setBitPosition(const uint8_t position) = 0;


        /** \brief Get the value's start bit position inside inside the CAN message */
        virtual uint8_t getStartBitPosition() const = 0;

        /** \brief Get the value's end bit position inside inside the CAN message */
        virtual uint8_t getEndBitPosition() const = 0;


        /** \brief Get the value's size in bits */
        virtual uint8_t getSizeInBits() const = 0;


        /** \brief Get the value */
        virtual const void* getValue() const = 0;

        /** \brief Update the value from a CAN message */
        virtual void updateValue(const CanMsg& can_msg) = 0;

};


#endif // ICANMSGVALUE_H
