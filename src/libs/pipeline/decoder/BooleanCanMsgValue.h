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

#ifndef BOOLEANCANMSGVALUE_H
#define BOOLEANCANMSGVALUE_H

#include "CanMsgValueBase.h"


/** \brief Boolean value */
class BooleanCanMsgValue : public CanMsgValueBase
{
    public:


        /** \brief Constructor */
        BooleanCanMsgValue();

        /** \brief Destructor */
        virtual ~BooleanCanMsgValue();


        /** \brief Get the value's type */
        virtual const std::string& getType() const override { return m_type; }


        /** \brief Get the value */
        virtual const void* getValue() const override { return &m_value; }

        /** \brief Update the value's from a CAN message */
        virtual void updateValue(const CanMsg& can_msg) override;


    private:

        /** \brief Type */
        static const std::string m_type;

        /** \brief Value */
        bool m_value;
};


#endif // BOOLEANCANMSGVALUE_H
