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

#ifndef INPUTCANMSGVALUECONVERTER_H
#define INPUTCANMSGVALUECONVERTER_H


/** \brief Interface for classes which converts input value of a CAN message value converter to a specific value type */
template <typename OutputType>
class IInputCanMsgValueConverter
{
    public:

        /** \brief Destructor */
        virtual ~IInputCanMsgValueConverter() {}


        /** \brief Convert a value to the specified output type */
        virtual OutputType convert(const void* value) const = 0;
};

/** \brief Class which converts input value of a CAN message value converter to to a specific value type  */
template <typename InputType, typename OutputType>
class InputCanMsgValueConverter : public IInputCanMsgValueConverter<OutputType>
{
    public:

        /** \brief Convert a value to the specified output type */
        virtual OutputType convert(const void* value) const override
        {
            const InputType& input_value = *reinterpret_cast<const InputType*>(value);
            return static_cast<OutputType>(input_value);
        }
};

#endif // INPUTCANMSGVALUECONVERTER_H
