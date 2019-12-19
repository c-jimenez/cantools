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

#ifndef FUNCTIONCANMSGVALUECONVERTER_H
#define FUNCTIONCANMSGVALUECONVERTER_H

#include "CanMsgValueConverterBase.h"
#include "InputCanMsgValueConverter.h"
#include "MathExpression.h"

#include <map>
#include <memory>


/** \brief Convert a CAN message value to a double precision floating point value using a custom transfer function */
class FunctionCanMsgValueConverter : public CanMsgValueConverterBase
{
    public:

        /** \brief Constructor */
        FunctionCanMsgValueConverter(const std::string& name, const std::string& type);

        /** \brief Copy constructor */
        FunctionCanMsgValueConverter(const FunctionCanMsgValueConverter& copy);

        /** \brief Destructor */
        virtual ~FunctionCanMsgValueConverter();


        /** \brief Set the associated CAN message value type */
        virtual bool setCanMsgValue(ICanMsgValue& can_value) override;

        /** \brief Convert the value from the associated CAN message value */
        virtual void convertValue() override;

        /** \brief Get the converted value */
        virtual const void* getValue() const override { return &m_value; }


        /** \brief Clone the converter */
        virtual ICanMsgValueConverter* clone() const;


        /** \brief Set the custom transfer function */
        void setTransferFunction(const std::string& transfer_function, std::unique_ptr<IMathExpression>& transfer_math_expr);


    private:

        /** \brief Transfer function */
        std::string m_transfer_function;

        /** \brief Transfer function as a mathematical expression */
        std::unique_ptr<IMathExpression> m_transfer_math_expr;

        /** \brief Value */
        double m_value;

        /** \brief CAN message value object */
        ICanMsgValue* m_can_value;

        /** \brief Input value converter */
        IInputCanMsgValueConverter<double>* m_input_converter;

        /** \brief Environment for CAN message value's evalutation */
        IMathExpression::MathExpressionEnv m_math_expr_env;

};


#endif // FUNCTIONCANMSGVALUECONVERTER_H
