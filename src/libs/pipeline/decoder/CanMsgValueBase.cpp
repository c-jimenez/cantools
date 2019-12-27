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

#include "CanMsgValueBase.h"

#include <cstring>

/** \brief Constructor */
CanMsgValueBase::CanMsgValueBase()
: m_can_msg(nullptr)
, m_byte_position(0)
, m_bit_position(0)
, m_size_in_bits(0)
, m_value_mask(0)
{}

/** \brief Destructor */
CanMsgValueBase::~CanMsgValueBase()
{}

/** \brief Set the value's byte position inside the CAN message */
bool CanMsgValueBase::setBytePosition(const uint8_t position)
{
    // Convert to bit position
    const uint8_t bit_position = position * 8u + m_bit_position;
    
    // Size of a CAN message in bits
    const uint8_t can_msg_size_in_bits = CAN_MSG_MAX_LEN * 8u;

    // Check if there is some place left in the message
    const bool ret = ((bit_position + m_size_in_bits) <= can_msg_size_in_bits);
    if (ret)
    {
        // Save value
        m_byte_position = position;
    }
    return ret;
}

/** \brief Set the value's byte position inside its first byte into the CAN message */
bool CanMsgValueBase::setBitPosition(const uint8_t position)
{
    // Convert to bit position
    const uint8_t bit_position = m_byte_position * 8u + position;

    // Size of a CAN message in bits
    const uint8_t can_msg_size_in_bits = CAN_MSG_MAX_LEN * 8u;

    // Check if there is some place left in the message
    const bool ret = ((bit_position + m_size_in_bits) <= can_msg_size_in_bits);
    if (ret)
    {
        // Save value
        m_bit_position = position;
    }
    return ret;
}

/** \brief Set the value's size in bits */
void CanMsgValueBase::setSizeInBits(const uint8_t size_in_bits)
{
    // Save value
    m_size_in_bits = size_in_bits;

    // Update value mask
    m_value_mask = (1ull << m_size_in_bits) - 1u;
}

/** \brief Retrieve the masked value */
void CanMsgValueBase::getMaskedValue(const CanMsg& can_msg, void* value)
{
    uint64_t value_64 = 0;
    for (int8_t i = CAN_MSG_MAX_LEN; i >= 0; i--)
    {
        value_64 = (value_64 << 8u) + can_msg.data[i];
    }
    value_64 = ((value_64 >> getStartBitPosition()) & m_value_mask);
    memcpy(value, &value_64, sizeof(uint64_t));
}
