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

#include "IntegerCanMsgValue.h"

/** \brief Constructor */
IntegerCanMsgValue::IntegerCanMsgValue()
: CanMsgValueBase()
, m_type("int")
, m_signed_value(true)
, m_svalue(0)
, m_uvalue(0)
{
    // Set bit size
    setSizeInBits(64u);
}

/** \brief Destructor */
IntegerCanMsgValue::~IntegerCanMsgValue()
{}

/** \brief Update the value from a CAN message */
void IntegerCanMsgValue::updateValue(const CanMsg& can_msg)
{
    if (m_signed_value)
    {
        int64_t inverted_value_mask = ~static_cast<uint64_t>(getBitMask());
        getMaskedValue(can_msg, &m_svalue);
        m_svalue |= inverted_value_mask;
    }
    else
    {
        getMaskedValue(can_msg, &m_uvalue);
    }
}

/** \brief Set the size in bits of the value */
bool IntegerCanMsgValue::setSizeInBits(const uint8_t size_in_bits)
{
    // Update size in bit
    uint8_t old_bit_size = getSizeInBits();
    CanMsgValueBase::setSizeInBits(size_in_bits);

    // Check if the new size can be applied
    bool ret = setBitPosition(getBitPosition());
    if (!ret)
    {
        // Revert size in bits
        CanMsgValueBase::setSizeInBits(old_bit_size);
    }

    return ret;
}
