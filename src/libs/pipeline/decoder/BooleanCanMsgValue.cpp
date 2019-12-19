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

#include "BooleanCanMsgValue.h"


/** \brief Type */
const std::string BooleanCanMsgValue::m_type = "bool";


/** \brief Constructor */
BooleanCanMsgValue::BooleanCanMsgValue()
: CanMsgValueBase()
, m_value(false)
{
    // Fixed bit size => 1 bit
    setSizeInBits(1u);
}

/** \brief Destructor */
BooleanCanMsgValue::~BooleanCanMsgValue()
{}

/** \brief Update the value from a CAN message */
void BooleanCanMsgValue::updateValue(const CanMsg& can_msg)
{
    m_value = ((can_msg.data[getBytePosition()] & (1u << getBitPosition())) != 0);
}
