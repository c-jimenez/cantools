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

#include "EnumCanMsgValueConverter.h"


using namespace std;



/** \brief Constructor */
EnumCanMsgValueConverter::EnumCanMsgValueConverter(const std::string& name, const std::string& type)
: CanMsgValueConverterBase(name, type, "string")
, m_value("")
, m_enum()
, m_can_value(nullptr)
{}

/** \brief Copy constructor */
EnumCanMsgValueConverter::EnumCanMsgValueConverter(const EnumCanMsgValueConverter& copy)
: CanMsgValueConverterBase(getName(), getType(), "string")
, m_value(copy.m_value)
, m_enum(copy.m_enum)
, m_can_value(nullptr)
{}

/** \brief Destructor */
EnumCanMsgValueConverter::~EnumCanMsgValueConverter()
{}


/** \brief Set the associated CAN message value type */
bool EnumCanMsgValueConverter::setCanMsgValue(ICanMsgValue& can_value)
{
    bool ret = false;

    // Check input type
    const string& input_type = can_value.getType();
    if ((input_type == "int") ||
        (input_type == "uint"))
    {
        m_can_value = &can_value;
        ret = true;
    }

    return ret;
}

/** \brief Convert the value from the associated CAN message value */
void EnumCanMsgValueConverter::convertValue()
{
    if (m_can_value != nullptr)
    {
        uint64_t value = m_input_converter.convert(m_can_value->getValue());
        auto iter = m_enum.find(value);
        if (iter == m_enum.end())
        {
            m_value = "";
        }
        else
        {
            m_value = iter->second;
        }
    }
}

/** \brief Clone the converter */
ICanMsgValueConverter* EnumCanMsgValueConverter::clone() const
{
    return new EnumCanMsgValueConverter(*this);
}

/** \brief Add a value to the enumerate */
void EnumCanMsgValueConverter::addValue(const int64_t value, const std::string& str_value)
{
    m_enum[value] = str_value;
}
