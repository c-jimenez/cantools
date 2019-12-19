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

#ifndef FLOATCANMSGVALUE_H
#define FLOATCANMSGVALUE_H

#include "CanMsgValueBase.h"


/** \brief Floating point value */
class FloatCanMsgValue : public CanMsgValueBase
{
    public:


        /** \brief Constructor */
        FloatCanMsgValue();

        /** \brief Destructor */
        virtual ~FloatCanMsgValue();


        /** \brief Get the value's type */
        virtual const std::string& getType() const override { return m_type; }


        /** \brief Get the value */
        virtual const void* getValue() const override { if (m_sp_float_value) return &m_sp_value; else return &m_dp_value; }

        /** \brief Update the value from a CAN message */
        virtual void updateValue(const CanMsg& can_msg) override;


        /** \brief Set the precision of the floating point value */
        bool setPrecision(const bool sp_float_value);


    private:

        /** \brief Type */
        std::string m_type;

        /** \brief Indicate if its a single precision floating point value */
        bool m_sp_float_value;

        /** \brief Value as single precision floating point value */
        float m_sp_value;

        /** \brief Value as double precision floating point value */
        double m_dp_value;
};


#endif // FLOATCANMSGVALUE_H
