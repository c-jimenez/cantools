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

#ifndef ICANMSGVALUECONVERTER_H
#define ICANMSGVALUECONVERTER_H

#include "ICanMsgValue.h"


/** \brief Interface for CAN message value converters implementations */
class ICanMsgValueConverter
{
    public:

        /** \brief Destructor */
        virtual ~ICanMsgValueConverter() {}


        /** \brief Get the converter's name */
        virtual const std::string& getName() const = 0;

        /** \brief Get the converter's type */
        virtual const std::string& getType() const = 0;

        /** \brief Get the converter's output type */
        virtual const std::string& getOutputType() const = 0;


        /** \brief Set the associated CAN message value type */
        virtual bool setCanMsgValue(ICanMsgValue& can_value) = 0;

        /** \brief Convert the value from the associated CAN message value */
        virtual void convertValue() = 0;

        /** \brief Get the converted value */
        virtual const void* getValue() const = 0;


        /** \brief Clone the converter */
        virtual ICanMsgValueConverter* clone() const = 0;
};


#endif // ICANMSGVALUECONVERTER_H
