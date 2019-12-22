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

#include "FloatCanMsgValue.h"

/** \brief Constructor */
FloatCanMsgValue::FloatCanMsgValue()
: CanMsgValueBase()
, m_type("float")
, m_sp_float_value(true)
, m_sp_value(0.f)
, m_dp_value(0.)
{
    // Set bit size
    setPrecision(true);
}

/** \brief Destructor */
FloatCanMsgValue::~FloatCanMsgValue()
{}

/** \brief Update the value from a CAN message */
void FloatCanMsgValue::updateValue(const CanMsg& can_msg)
{
    if (m_sp_float_value)
    {
        getMaskedValue(can_msg, &m_sp_value);
    }
    else
    {
        getMaskedValue(can_msg, &m_dp_value);
    }
}

/** \brief Set the precision of the floating point value */
bool FloatCanMsgValue::setPrecision(const bool sp_float_value)
{
    // Update size in bit
    uint8_t old_bit_size = getSizeInBits();
    if (sp_float_value)
    {
        setSizeInBits(32u);
    }
    else
    {
        setSizeInBits(64u);
    }

    // Check if the new size can be applied
    bool ret = setBitPosition(getBitPosition());
    if (ret)
    {
        // Apply new precision
        m_sp_float_value = sp_float_value;
        if (m_sp_float_value)
        {
            m_type = "float";
        }
        else 
        {
            m_type = "double";
        }
    }
    else
    {
        // Revert size in bits
        setSizeInBits(old_bit_size);
    }

    return ret;
}
