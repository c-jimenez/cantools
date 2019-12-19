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

#ifndef INTEGERCANMSGVALUE_H
#define INTEGERCANMSGVALUE_H

#include "CanMsgValueBase.h"


/** \brief Integer value */
class IntegerCanMsgValue : public CanMsgValueBase
{
    public:


        /** \brief Constructor */
        IntegerCanMsgValue();

        /** \brief Destructor */
        virtual ~IntegerCanMsgValue();


        /** \brief Get the value's type */
        virtual const std::string& getType() const override { return m_type; }


        /** \brief Get the value */
        virtual const void* getValue() const override { if (m_signed_value) return &m_svalue; else return &m_uvalue; }

        /** \brief Update the value from a CAN message */
        virtual void updateValue(const CanMsg& can_msg) override;


        /** \brief Set the signedness of the value */
        void setSignedness(const bool signed_value) { m_signed_value = signed_value; if (m_signed_value) m_type = "int"; else m_type = "uint"; }

        /** \brief Set the size in bits of the value */
        bool setSizeInBits(const uint8_t size_in_bits);


    private:

        /** \brief Type */
        std::string m_type;

        /** \brief Indicate if its a signed value */
        bool m_signed_value;

        /** \brief Value as signed integer value */
        int64_t m_svalue;

        /** \brief Value as double precision floating point value */
        uint64_t m_uvalue;
};


#endif // INTEGERCANMSGVALUE_H
