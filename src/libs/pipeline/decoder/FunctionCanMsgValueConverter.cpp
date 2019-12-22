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

#include "FunctionCanMsgValueConverter.h"
#include "MathExpressionParser.h"

using namespace std;


/** \brief Constructor */
FunctionCanMsgValueConverter::FunctionCanMsgValueConverter(const std::string& name, const std::string& type)
: CanMsgValueConverterBase(name, type, "double")
, m_transfer_function("")
, m_transfer_math_expr()
, m_value(0.)
, m_can_value(nullptr)
, m_input_converter(nullptr)
, m_math_expr_env()
{}

/** \brief Copy constructor */
FunctionCanMsgValueConverter::FunctionCanMsgValueConverter(const FunctionCanMsgValueConverter& copy)
: CanMsgValueConverterBase(getName(), getType(), "double")
, m_transfer_function(copy.m_transfer_function)
, m_transfer_math_expr()
, m_value(copy.m_value)
, m_can_value(nullptr)
, m_input_converter(nullptr)
, m_math_expr_env(copy.m_math_expr_env)
{
    MathExpressionParser parser;
    parser.parse(m_transfer_function, m_transfer_math_expr);
}

/** \brief Destructor */
FunctionCanMsgValueConverter::~FunctionCanMsgValueConverter()
{
    delete m_input_converter;
}


/** \brief Set the associated CAN message value type */
bool FunctionCanMsgValueConverter::setCanMsgValue(ICanMsgValue& can_value)
{
    bool ret = true;
    IInputCanMsgValueConverter<double>* input_converter = nullptr;

    // Check input type
    const string& input_type = can_value.getType();
    if (input_type == "int")
    {
        input_converter = new InputCanMsgValueConverter<int64_t, double>();
    }
    else if (input_type == "uint")
    {
        input_converter = new InputCanMsgValueConverter<uint64_t, double>();
    }
    else if (input_type == "float")
    {
        input_converter = new InputCanMsgValueConverter<float, double>();
    }
    else if(input_type == "double")
    {
        input_converter = new InputCanMsgValueConverter<double, double>();
    }
    else
    {
        ret = false;
    }
    if (ret)
    {
        // Save CAN message value and input converter
        m_can_value = &can_value;
        delete m_input_converter;
        m_input_converter = input_converter;
    }

    return ret;
}

/** \brief Convert the value from the associated CAN message value */
void FunctionCanMsgValueConverter::convertValue()
{
    if (m_can_value != nullptr)
    {
        m_math_expr_env["in_val"] = m_input_converter->convert(m_can_value->getValue());
        m_value = m_transfer_math_expr->evaluate(m_math_expr_env);
    }
}

/** \brief Clone the converter */
ICanMsgValueConverter* FunctionCanMsgValueConverter::clone() const
{
    return new FunctionCanMsgValueConverter(*this);
}

/** \brief Set the custom transfer function */
void FunctionCanMsgValueConverter::setTransferFunction(const std::string& transfer_function, std::unique_ptr<IMathExpression>& transfer_math_expr)
{
    m_transfer_function = transfer_function;
    m_transfer_math_expr.swap(transfer_math_expr);
}
