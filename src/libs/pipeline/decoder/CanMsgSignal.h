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

#ifndef CANMSGSIGNAL_H
#define CANMSGSIGNAL_H

#include "ICanMsgValue.h"
#include "ICanMsgValueConverter.h"

#include <string>


/** \brief Represent a signal transmitted over a CAN message */
class CanMsgSignal
{
    public:

        /** \brief Constructor */
        CanMsgSignal();

        /** \brief Destructor */
        virtual ~CanMsgSignal();



        /** \brief Get the signal's name */
        const std::string& getName() const { return m_name; }

        /** \brief Set the signal's name */
        void setName(const std::string& name) { m_name = name; }


        /** \brief Get the signal's unit */
        const std::string& getUnit() const { return m_unit; }

        /** \brief Set the signal's unit */
        void setUnit(const std::string& unit) { m_unit = unit; }


        /** \brief Get the raw value type */
        const std::string& getRawValueType() const { return m_raw_type; }

        /** \brief Get the converted value type */
        const std::string& getConvertedValueType() const { return m_converted_type; }


        /** \brief Set the signal's value */
        bool setValue(ICanMsgValue& can_value);

        /** \brief Set the signal's converter */
        bool setConverter(ICanMsgValueConverter& converter);


        /** \brief Get the raw value */
        const void* getRawValue() const;

        /** \brief Get the converted value */
        const void* getConvertedValue() const;

        /** \brief Get the raw value */
        template <typename ValueType>
        const ValueType& getRawValue() const { return *reinterpret_cast<const ValueType*>(m_can_value->getValue()); }

        /** \brief Get the converted value */
        template <typename ValueType>
        const ValueType& getConvertedValue() const { return *reinterpret_cast<const ValueType*>(m_converter->getValue()); }

        /** \brief Update the raw and converted values from a CAN message */
        void updateValues(const CanMsg& can_msg);


    private:

        /** \brief Name */
        std::string m_name;

        /** \brief Unit */
        std::string m_unit;

        /** \brief Raw value type */
        std::string m_raw_type;

        /** \brief Converted value type */
        std::string m_converted_type;

        /** \brief Value */
        ICanMsgValue* m_can_value;

        /** \brief Converter */
        ICanMsgValueConverter* m_converter;

};


#endif // CANMSGSIGNAL_H
