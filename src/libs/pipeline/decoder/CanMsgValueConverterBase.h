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

#ifndef CANMSGVALUECONVERTERBASE_H
#define CANMSGVALUECONVERTERBASE_H

#include "ICanMsgValueConverter.h"


/** \brief Base class for CAN message value converters implementations */
class CanMsgValueConverterBase : public ICanMsgValueConverter
{
    public:

        /** \brief Destructor */
        CanMsgValueConverterBase(const std::string& name, const std::string& type, const std::string& output_type)
        : m_name(name)
        , m_type(type)
        , m_output_type(output_type)
        {}

        /** \brief Destructor */
        virtual ~CanMsgValueConverterBase() {}


        /** \brief Get the converter's name */
        virtual const std::string& getName() const override { return m_name; }

        /** \brief Get the converter's type */
        virtual const std::string& getType() const override { return m_type; }

        /** \brief Get the converter's output type */
        virtual const std::string& getOutputType() const override { return m_output_type; }


    private:

        /** \brief Converter's name */
        const std::string m_name;

        /** \brief Converter's type */
        const std::string m_type;

        /** \brief Converter's output type */
        const std::string m_output_type;
};


#endif // CANMSGVALUECONVERTERBASE_H
