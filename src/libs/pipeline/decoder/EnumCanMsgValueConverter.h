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

#ifndef ENUMCANMSGVALUECONVERTER_H
#define ENUMCANMSGVALUECONVERTER_H

#include "CanMsgValueConverterBase.h"
#include "InputCanMsgValueConverter.h"

#include <map>


/** \brief Convert a CAN message value to a string */
class EnumCanMsgValueConverter : public CanMsgValueConverterBase
{
    public:

        /** \brief Constructor */
        EnumCanMsgValueConverter(const std::string& name, const std::string& type);

        /** \brief Copy constructor */
        EnumCanMsgValueConverter(const EnumCanMsgValueConverter& copy);

        /** \brief Destructor */
        virtual ~EnumCanMsgValueConverter();


        /** \brief Set the associated CAN message value type */
        virtual bool setCanMsgValue(ICanMsgValue& can_value) override;

        /** \brief Convert the value from the associated CAN message value */
        virtual void convertValue() override;

        /** \brief Get the converted value */
        virtual const void* getValue() const override { return &m_value; }


        /** \brief Clone the converter */
        virtual ICanMsgValueConverter* clone() const;



        /** \brief Add a value to the enumerate */
        void addValue(const int64_t value, const std::string& str_value);


    private:

        /** \brief Value */
        std::string m_value;

        /** \brief Enumerate */
        std::map<int64_t, std::string> m_enum;

        /** \brief CAN message value object */
        ICanMsgValue* m_can_value;

        /** \brief Input value converter */
        InputCanMsgValueConverter<uint64_t, uint64_t> m_input_converter;
};


#endif // ENUMCANMSGVALUECONVERTER_H
