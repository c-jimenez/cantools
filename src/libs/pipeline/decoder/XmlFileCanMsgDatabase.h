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

#ifndef XMLMESSAGEDATABASEFILE_H
#define XMLMESSAGEDATABASEFILE_H

#include "ICanMsgDatabase.h"
#include "CanMsgSignal.h"
#include "ICanMsgValueConverter.h"

#include <string>
#include <memory>

class EnumCanMsgValueConverter;
class FunctionCanMsgValueConverter;

/** \brief CAN message database stored in an XML file */
class XmlFileCanMsgDatabase : public ICanMsgDatabase
{
    public:

        /** \brief Constructor */
        XmlFileCanMsgDatabase();

        /** \brief Destructor */
        virtual ~XmlFileCanMsgDatabase();


        /** \brief Load the message database from the file */
        bool load(const std::string& xml_file);


        /** \brief Get the list of CAN messages */
        virtual const std::map<uint32_t, std::shared_ptr<CanMsgDesc>>& getCanMsgs() const override { return m_can_msgs; }


        /** \brief Get the last error string */
        const std::string& getLastError() { return m_last_error; }

    private:

        /** \brief Get the last error string */
        std::string m_last_error;

        /** \brief List of CAN messages */
        std::map<uint32_t, std::shared_ptr<CanMsgDesc>> m_can_msgs;

        /** \brief List of CAN messages values */
        std::map<std::string, std::shared_ptr<ICanMsgValue>> m_can_msg_values;

        /** \brief List of CAN messages values converters */
        std::map<std::string, std::shared_ptr<ICanMsgValueConverter>> m_can_msg_value_converters;

        /** \brief List of cloned CAN messages values converters */
        std::vector<std::shared_ptr<ICanMsgValueConverter>> m_cloned_can_msg_value_converters;

        /** \brief List of CAN messages signals */
        std::map<std::string, std::shared_ptr<CanMsgSignal>> m_can_msg_signals;


        /** \brief Set the error message based on an xml node */
        void setErrorMessage(void* xml_node, const char* error_msg);

        /** \brief Convert a string value representing either a decimal or an hexadecimal unsigned integer value */
        bool toUint(const void* attribute, uint64_t& value);

        /** \brief Convert a string value representing either a decimal or an hexadecimal integer value */
        bool toInt(const void* attribute, int64_t& value);

        /** \brief Load the list of CAN messages */
        bool loadMessages(void* root_node);

        /** \brief Load the list of CAN message values */
        bool loadValues(void* root_node, CanMsgDesc& can_msg);

        /** \brief Load the list of CAN values converters */
        bool loadConverters(void* root_node);

        /** \brief Load an enumeration CAN values converter */
        bool loadEnumConverter(EnumCanMsgValueConverter& converter, void* root_node);

        /** \brief Load a function CAN values converter */
        bool loadFunctionConverter(FunctionCanMsgValueConverter& converter, void* root_node);

        /** \brief Load the list of CAN signals */
        bool loadSignals(void* root_node);

};


#endif // XMLMESSAGEDATABASEFILE_H
