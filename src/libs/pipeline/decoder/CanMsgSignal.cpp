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

#include "CanMsgSignal.h"



/** \brief Constructor */
CanMsgSignal::CanMsgSignal()
: m_name("")
, m_unit("")
, m_raw_type("")
, m_converted_type("")
, m_can_value(nullptr)
, m_converter(nullptr)
{}

/** \brief Destructor */
CanMsgSignal::~CanMsgSignal()
{}

/** \brief Set the signal's value */
bool CanMsgSignal::setValue(ICanMsgValue& can_value)
{
    bool ret = true;

    if (m_converter != nullptr)
    {
        ret = m_converter->setCanMsgValue(can_value);
    }
    if (ret)
    {
        m_can_value = &can_value;
        m_raw_type = m_can_value->getType();
        if (m_converter == nullptr)
        {
            m_converted_type = m_raw_type;
        }
    }

    return ret;
}

/** \brief Set the signal's converter */
bool CanMsgSignal::setConverter(ICanMsgValueConverter& converter)
{
    bool ret = true;

    if (m_can_value != nullptr)
    {
        ret = converter.setCanMsgValue(*m_can_value);
    }
    if (ret)
    {
        m_converter = &converter;
        m_converted_type = m_converter->getOutputType();
    }

    return ret;
}

/** \brief Get the raw value */
const void* CanMsgSignal::getRawValue() const
{
    const void* ret = nullptr;

    if (m_can_value != nullptr)
    {
        ret = m_can_value->getValue();
    }

    return ret;
}

/** \brief Get the converted value */
const void* CanMsgSignal::getConvertedValue() const
{
    const void* ret = nullptr;

    if (m_converter != nullptr)
    {
        ret = m_converter->getValue();
    }
    else
    {
        if (m_can_value != nullptr)
        {
            ret = m_can_value->getValue();
        }
    }

    return ret;
}

/** \brief Update the raw and converted values from a CAN message */
void CanMsgSignal::updateValues(const CanMsg& can_msg)
{
    // Update the raw value
    if (m_can_value != nullptr)
    {
        m_can_value->updateValue(can_msg);

        // Update the converted value
        if (m_converter != nullptr)
        {
            m_converter->convertValue();
        }
    }
}
