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

#ifndef DECODEDCANMSGSTREAMLOGGER_H
#define DECODEDCANMSGSTREAMLOGGER_H

#include "CanMsgDecoder.h"

#include <ostream>


/** \brief Utility class to log received decoded messages to a stream */
class DecodedCanMsgStreamLogger : public ICanMsgDecoderListener
{
    public:


        /** \brief Output modes */
        enum OutputMode
        {
            /** \brief Readable text */
            OUT_TEXT = 0,
            /** \brief CSV */
            OUT_CSV = 1u
        };


        /** \brief Constructor */
        DecodedCanMsgStreamLogger();

        /** \brief Destructor */
        virtual ~DecodedCanMsgStreamLogger();


        /** \brief Set the output options */
        void setOutputOptions(std::ostream& output_stream, const OutputMode output_mode);


        /** \brief Called when message has been decoded */
        virtual void canMsgDecoded(const CanMsg& can_msg, const CanMsgDesc& decoded_can_msg) override;


    
    private:

        /** \brief Output stream */
        std::ostream* m_output_stream;

        /** \brief Output modes */
        OutputMode m_output_mode;


        /** \brief Output data in text mode */
        void textOutput(const CanMsg& can_msg, const CanMsgDesc& decoded_can_msg);

        /** \brief Output data in CSV mode */
        void csvOutput(const CanMsg& can_msg, const CanMsgDesc& decoded_can_msg);

        /** \brief Outputs a value to the stream */
        void printValue(const void* value, const std::string& value_type);
};


#endif // DECODEDCANMSGSTREAMLOGGER_H
