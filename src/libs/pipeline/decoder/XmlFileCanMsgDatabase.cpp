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

#include "XmlFileCanMsgDatabase.h"
#include "VectorHelper.h"
#include "tinyxml2.h"

#include "BooleanCanMsgValue.h"
#include "IntegerCanMsgValue.h"
#include "FloatCanMsgValue.h"

#include "EnumCanMsgValueConverter.h"
#include "FunctionCanMsgValueConverter.h"
#include "MathExpressionParser.h"

#include <sstream>

using namespace std;
using namespace tinyxml2;



/** \brief Constructor */
XmlFileCanMsgDatabase::XmlFileCanMsgDatabase()
: m_last_error("")
, m_can_msgs()
, m_can_msg_values()
, m_can_msg_value_converters()
, m_cloned_can_msg_value_converters()
, m_can_msg_signals()
{}

/** \brief Destructor */
XmlFileCanMsgDatabase::~XmlFileCanMsgDatabase()
{}


/** \brief Load the message database from the file */
bool XmlFileCanMsgDatabase::load(const std::string& xml_file)
{
    bool ret = false;

    // Open file
    XMLDocument xml_doc;
    XMLError error = xml_doc.LoadFile(xml_file.c_str());
    if (error == XML_SUCCESS)
    {
        // Look for root node
        XMLElement* root_node = xml_doc.FirstChildElement("message_db");
        if (root_node == nullptr)
        {
            m_last_error = "Unable to find root XML node";
        }
        else
        {
            // Load messages
            XMLElement* messages_node = root_node->FirstChildElement("messages");
            if (messages_node == nullptr)
            {
                m_last_error = "Unable to find messages XML node";
            }
            else
            {
                ret = loadMessages(messages_node);
                if (ret)
                {
                    // Load converters
                    XMLElement* converters_node = root_node->FirstChildElement("converters");
                    if (converters_node != nullptr)
                    {
                        ret = loadConverters(converters_node);
                    }
                    if (ret)
                    {
                        // Load signals
                        XMLElement* signals_node = root_node->FirstChildElement("signals");
                        if (signals_node == nullptr)
                        {
                            m_last_error = "Unable to find signals XML node";
                        }
                        else
                        {
                            ret = loadSignals(signals_node);
                        }
                    }
                }
            }
        }
    }
    else
    {
        m_last_error = xml_doc.ErrorStr();
    }

    return ret;
}

/** \brief Set the error message based on an xml node */
void XmlFileCanMsgDatabase::setErrorMessage(void* xml_node, const char* error_msg)
{
    XMLElement* error_node = reinterpret_cast<XMLElement*>(xml_node);
    stringstream ss_error_msg;
    ss_error_msg << "Element : " << error_node->Value() << " - Line : " << error_node->GetLineNum() << " => " << error_msg;
    m_last_error = ss_error_msg.str();
}

/** \brief Convert a string value representing either a decimal or an hexadecimal unsigned integer value */
bool XmlFileCanMsgDatabase::toUint(const void* attribute, uint64_t& value)
{
    const XMLAttribute* value_attribute = reinterpret_cast<const XMLAttribute*>(attribute);
    string str_value = value_attribute->Value();

    // Check hexadecimal value
    if ((str_value.size() > 2) &&
        (str_value.substr(0u, 2u) == "0x"))
    {
        value = std::stoull(str_value.substr(2u, str_value.size() - 2u), 0, 16);
    }
    else
    {
        value = std::stoull(str_value);
    }
    return true;
}

/** \brief Convert a string value representing either a decimal or an hexadecimal integer value */
bool XmlFileCanMsgDatabase::toInt(const void* attribute, int64_t& value)
{
    const XMLAttribute* value_attribute = reinterpret_cast<const XMLAttribute*>(attribute);
    string str_value = value_attribute->Value();

    // Check hexadecimal value
    if ((str_value.size() > 2) &&
        (str_value.substr(0u, 2u) == "0x"))
    {
        value = std::stoll(str_value.substr(2u, str_value.size() - 2u), 0, 16);
    }
    else
    {
        value = std::stoll(str_value);
    }
    return true;
}

/** \brief Load the list of CAN messages */
bool XmlFileCanMsgDatabase::loadMessages(void* root_node)
{
    bool ret = true;
    XMLElement* messages_node = reinterpret_cast<XMLElement*>(root_node);

    XMLElement* message_node = messages_node->FirstChildElement("message");
    while (ret && (message_node != nullptr))
    {
        // Extract id
        uint64_t id = 0;
        const XMLAttribute* id_attribute = message_node->FindAttribute("id");
        if (id_attribute == nullptr)
        {
            ret = false;
            setErrorMessage(message_node, "No id attribute defined for the CAN message");
        }
        else
        {
            ret = toUint(id_attribute, id);
            if (!ret)
            {
                setErrorMessage(message_node, "Invalid value for the CAN message's id");
            }
        }

        // Extract name
        std::string name = "";
        const XMLAttribute* name_attribute = message_node->FindAttribute("name");
        if (name_attribute != nullptr)
        {
            name = name_attribute->Value();
        }

        // Extract extended flag
        bool extended = false;
        const XMLAttribute* extended_attribute = message_node->FindAttribute("extended");
        if (extended_attribute != nullptr)
        {
            if (extended_attribute->QueryBoolValue(&extended) != XML_SUCCESS)
            {
                ret = false;
                setErrorMessage(message_node, "Extended attribute of a CAN message must be a boolean value");
            }
        }

        // Create CAN message description
        if (ret)
        {
            CanMsgDesc* can_msg = new CanMsgDesc(static_cast<uint32_t>(id), name, extended);

            // Load values
            XMLElement* values_node = message_node->FirstChildElement("values");
            if (values_node != nullptr)
            {
                ret = loadValues(values_node, *can_msg);
            }

            // Add message to the list
            auto sp = shared_ptr<CanMsgDesc>(can_msg);
            m_can_msgs[sp->getId()] = sp;
        }

        // Next message
        message_node = message_node->NextSiblingElement();
    }

    return ret;
}

/** \brief Load the list of CAN message values */
bool XmlFileCanMsgDatabase::loadValues(void* root_node, CanMsgDesc& can_msg)
{
    bool ret = true;
    XMLElement* values_node = reinterpret_cast<XMLElement*>(root_node);

    XMLElement* value_node = values_node->FirstChildElement("value");
    while (ret && (value_node != nullptr))
    {
        // Extract id
        std::string id = "";
        const XMLAttribute* id_attribute = value_node->FindAttribute("id");
        if (id_attribute != nullptr)
        {
            id = id_attribute->Value();
        }
        else
        {
            ret = false;
            setErrorMessage(value_node, "No id attribute defined for the CAN message value");
        }

        // Extract type
        std::string type = "";
        const XMLAttribute* type_attribute = value_node->FindAttribute("type");
        if (type_attribute != nullptr)
        {
            type = type_attribute->Value();
        }
        else
        {
            ret = false;
            setErrorMessage(value_node, "No type attribute defined for the CAN message value");
        }

        // Extract byte position
        uint64_t byte_position = 0;
        const XMLAttribute* byte_position_attribute = value_node->FindAttribute("byte_position");
        if (byte_position_attribute == nullptr)
        {
            ret = false;
            setErrorMessage(value_node, "No byte_position attribute defined for the CAN message value");
        }
        else
        {
            ret = toUint(byte_position_attribute, byte_position);
            if (!ret)
            {
                setErrorMessage(value_node, "Invalid value for the CAN message value's byte position");
            }
        }

        // Extract bit position
        uint64_t bit_position = 0;
        const XMLAttribute* bit_position_attribute = value_node->FindAttribute("bit_position");
        if (bit_position_attribute == nullptr)
        {
            ret = false;
            setErrorMessage(value_node, "No bit_position attribute defined for the CAN message value");
        }
        else
        {
            ret = toUint(bit_position_attribute, bit_position);
            if (!ret)
            {
                setErrorMessage(value_node, "Invalid value for the CAN message value's bit position");
            }
        }

        // Extract size in bits
        uint64_t size_in_bits = 0;
        const XMLAttribute* size_in_bits_attribute = value_node->FindAttribute("size_in_bits");
        if (size_in_bits_attribute != nullptr)
        {
            ret = toUint(size_in_bits_attribute, size_in_bits);
            if (!ret || (size_in_bits == 0))
            {
                ret = false;
                setErrorMessage(value_node, "Invalid value for the CAN message value's size in bits");
            }
        }

        // Create value
        ICanMsgValue* value = nullptr;
        if (type == "bool")
        {
            value = new BooleanCanMsgValue();
        }
        else if (type == "uint")
        {
            IntegerCanMsgValue* int_value = new IntegerCanMsgValue();
            int_value->setSignedness(false);
            int_value->setSizeInBits(static_cast<uint8_t>(size_in_bits));
            value = int_value;
        }
        else if (type == "int")
        {
            IntegerCanMsgValue* int_value = new IntegerCanMsgValue();
            int_value->setSignedness(true);
            int_value->setSizeInBits(static_cast<uint8_t>(size_in_bits));
            value = int_value;
        }
        else if (type == "float")
        {
            FloatCanMsgValue* float_value = new FloatCanMsgValue();
            float_value->setPrecision(true);
            value = float_value;
        }
        else if (type == "double")
        {
            FloatCanMsgValue* float_value = new FloatCanMsgValue();
            float_value->setPrecision(false);
            value = float_value;
        }
        else
        {
            ret = false;
            setErrorMessage(value_node, "Invalid value for the CAN message value's type");
        }
        if (ret)
        {
            // Set generic attributes
            ret = value->setBytePosition(static_cast<uint8_t>(byte_position));
            if (!ret)
            {
                setErrorMessage(value_node, "Invalid value for the CAN message value's byte position");
            }
            ret = value->setBitPosition(static_cast<uint8_t>(bit_position));
            if (!ret)
            {
                setErrorMessage(value_node, "Invalid value for the CAN message value's bit position");
            }

            // Add value to CAN message
            auto sp = shared_ptr<ICanMsgValue>(value);
            ret = can_msg.addValue(sp);
            if (!ret)
            {
                setErrorMessage(value_node, "The value for the CAN message overlaps another value");
            }
            else
            {
                m_can_msg_values[id] = sp;
            }
        }

        // Next value
        value_node = value_node->NextSiblingElement();
    }

    return ret;
}

/** \brief Load the list of CAN values converters */
bool XmlFileCanMsgDatabase::loadConverters(void* root_node)
{
    bool ret = true;
    XMLElement* converters_node = reinterpret_cast<XMLElement*>(root_node);

    XMLElement* converter_node = converters_node->FirstChildElement("converter");
    while (ret && (converter_node != nullptr))
    {
        // Extract id
        std::string id = "";
        const XMLAttribute* id_attribute = converter_node->FindAttribute("id");
        if (id_attribute != nullptr)
        {
            id = id_attribute->Value();
        }
        else
        {
            ret = false;
            setErrorMessage(converter_node, "No id attribute defined for the CAN values converter");
        }

        // Extract type
        std::string type = "";
        const XMLAttribute* type_attribute = converter_node->FindAttribute("type");
        if (type_attribute != nullptr)
        {
            type = type_attribute->Value();
        }
        else
        {
            ret = false;
            setErrorMessage(converter_node, "No type attribute defined for the CAN values converter");
        }

        // Extract comment
        std::string comment = "";
        const XMLAttribute* comment_attribute = converter_node->FindAttribute("comment");
        if (comment_attribute != nullptr)
        {
            comment = comment_attribute->Value();
        }

        // Create CAN values converter
        if (ret)
        {
            ICanMsgValueConverter* converter = nullptr;
            if (type == "enum_converter")
            {
                EnumCanMsgValueConverter* enum_converter = new EnumCanMsgValueConverter(id, type);
                ret = loadEnumConverter(*enum_converter, converter_node);
                if (ret)
                {
                    converter = enum_converter;
                }
            }
            else if (type == "function_converter")
            {
                FunctionCanMsgValueConverter* function_converter = new FunctionCanMsgValueConverter(id, type);
                ret = loadFunctionConverter(*function_converter, converter_node);
                if (ret)
                {
                    converter = function_converter;
                }
            }
            else
            {
                ret = false;
                setErrorMessage(converter_node, "Unknown type attribute's value for the CAN values converter");
            }
            if (ret && (converter != nullptr))
            {
                // Add converter to the list
                auto sp = shared_ptr<ICanMsgValueConverter>(converter);
                m_can_msg_value_converters[id] = sp;
            }
            else
            {
                delete converter;
            }
        }

        // Next converter
        converter_node = converter_node->NextSiblingElement();
    }

    return ret;
}

/** \brief Load an enumeration CAN values converter */
bool XmlFileCanMsgDatabase::loadEnumConverter(EnumCanMsgValueConverter& converter, void* root_node)
{
    bool ret = true;
    XMLElement* converter_node = reinterpret_cast<XMLElement*>(root_node);

    XMLElement* enum_node = converter_node->FirstChildElement("enum");
    while (ret && (enum_node != nullptr))
    {
        // Extract id
        int64_t id = 0;
        const XMLAttribute* id_attribute = enum_node->FindAttribute("id");
        if (id_attribute == nullptr)
        {
            ret = false;
            setErrorMessage(enum_node, "No id attribute defined for the CAN values converter enumeration");
        }
        else
        {
            ret = toInt(id_attribute, id);
            if (!ret)
            {
                setErrorMessage(enum_node, "Invalid value for the CAN vaues converter enumeration's id");
            }
        }

        // Extract value
        std::string value = "";
        const XMLAttribute* value_attribute = enum_node->FindAttribute("value");
        if (value_attribute != nullptr)
        {
            value = value_attribute->Value();
        }
        else
        {
            ret = false;
            setErrorMessage(enum_node, "No value attribute defined for the CAN values converter enumeration");
        }

        // Add enum
        if (ret)
        {
            converter.addValue(id, value);
        }

        // Next enumeration
        enum_node = enum_node->NextSiblingElement();
    }

    return ret;
}

/** \brief Load a function CAN values converter */
bool XmlFileCanMsgDatabase::loadFunctionConverter(FunctionCanMsgValueConverter& converter, void* root_node)
{
    bool ret = true;
    XMLElement* converter_node = reinterpret_cast<XMLElement*>(root_node);

    XMLElement* function_node = converter_node->FirstChildElement("function");
    if (function_node == nullptr)
    {
        ret = false;
        setErrorMessage(converter_node, "Missing function node for the function CAN values converter");
    }
    else
    {
        // Extract function
        std::string function = "";
        const XMLAttribute* function_attribute = function_node->FindAttribute("f");
        if (function_attribute != nullptr)
        {
            function = function_attribute->Value();
        }
        else
        {
            ret = false;
            setErrorMessage(function_node, "No f attribute defined for the function CAN values converter");
        }

        // Build function expression
        if (ret)
        {
            MathExpressionParser math_parser;
            unique_ptr<IMathExpression> math_expr;
            ret = math_parser.parse(function, math_expr);
            if (ret)
            {
                converter.setTransferFunction(function, math_expr);
            }
            else
            {
                stringstream error;
                error << "Invalid syntax in the transfer function of the function CAN values converter : ";
                error << math_parser.getLastError();
                setErrorMessage(function_node, error.str().c_str());
            }
        }
    }

    return ret;
}

/** \brief Load the list of CAN signals */
bool XmlFileCanMsgDatabase::loadSignals(void* root_node)
{
    bool ret = true;
    XMLElement* signals_node = reinterpret_cast<XMLElement*>(root_node);

    XMLElement* signal_node = signals_node->FirstChildElement("signal");
    while (ret && (signal_node != nullptr))
    {
        // Extract id
        std::string id = "";
        const XMLAttribute* id_attribute = signal_node->FindAttribute("id");
        if (id_attribute != nullptr)
        {
            id = id_attribute->Value();
        }
        else
        {
            ret = false;
            setErrorMessage(signal_node, "No id attribute defined for the CAN signal");
        }

        // Extract value
        std::string value = "";
        const XMLAttribute* value_attribute = signal_node->FindAttribute("value");
        if (value_attribute != nullptr)
        {
            value = value_attribute->Value();
            if (m_can_msg_values.find(value) == m_can_msg_values.end())
            {
                ret = false;
                setErrorMessage(signal_node, "Unknown CAN value for the CAN signal");
            }
        }
        else
        {
            ret = false;
            setErrorMessage(signal_node, "No value attribute defined for the CAN signal");
        }

        // Extract converter
        std::string converter = "";
        const XMLAttribute* converter_attribute = signal_node->FindAttribute("converter");
        if (converter_attribute != nullptr)
        {
            converter = converter_attribute->Value();
            if (!converter.empty() &&
                (m_can_msg_value_converters.find(converter) == m_can_msg_value_converters.end()))
            {
                ret = false;
                setErrorMessage(signal_node, "Unknown CAN values converter for the CAN signal");
            }
        }

        // Extract unit
        std::string unit = "";
        const XMLAttribute* unit_attribute = signal_node->FindAttribute("unit");
        if (unit_attribute != nullptr)
        {
            unit = unit_attribute->Value();
        }

        // Create CAN signal
        if (ret)
        {
            CanMsgSignal* signal = new CanMsgSignal();
            signal->setName(id);
            signal->setUnit(unit);

            ICanMsgValue& can_value = *(m_can_msg_values[value]);
            ret = signal->setValue(can_value);
            if (ret)
            {
                if (!converter.empty())
                {
                    auto msg_converter = shared_ptr<ICanMsgValueConverter>(m_can_msg_value_converters[converter]->clone());
                    ret = signal->setConverter(*msg_converter);
                    if (!ret)
                    {
                        setErrorMessage(signal_node, "Incompatible CAN value type and CAN values converter type for the CAN signal");
                    }
                    else
                    {
                        m_cloned_can_msg_value_converters.push_back(msg_converter);
                    }
                }
            }
            else
            {
                setErrorMessage(signal_node, "Incompatible CAN value type for the CAN signal");
            }
            if (ret)
            {
                // Add signal to the list
                auto sp = shared_ptr<CanMsgSignal>(signal);
                m_can_msg_signals[id] = sp;

                CanMsgDesc* can_msg = can_value.getCanMsg();
                can_msg->addSignal(sp);
            }
            else
            {
                delete signal;
            }
        }

        // Next signal
        signal_node = signal_node->NextSiblingElement();
    }

    return ret;
}