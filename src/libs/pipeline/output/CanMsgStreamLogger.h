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

#ifndef CANMSGSTREAMLOGGER_H
#define CANMSGSTREAMLOGGER_H

#include "CanPipelineStageBase.h"

#include <ostream>

class CanController;

/** \brief Utility class to log received messages to a stream */
class CanMsgStreamLogger : public CanPipelineStageBase
{
    public:


        /** \brief Output modes */
        enum OutputMode
        {
            /** \brief Readable text */
            OUT_TEXT = 0,
            /** \brief CSV */
            OUT_CSV = 1u,
            /** \brief Binary */
            OUT_BINARY = 2u
        };


        /** \brief Constructor */
        CanMsgStreamLogger();

        /** \brief Destructor */
        virtual ~CanMsgStreamLogger();


        /** \brief Set the output options */
        void setOutputOptions(std::ostream& output_stream, const OutputMode output_mode, const CanController& can_controller);


    protected:

        /** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
        virtual bool processCanData(const CanData& can_data) override;


    private:

        /** \brief Output stream */
        std::ostream* m_output_stream;

        /** \brief Output modes */
        OutputMode m_output_mode;

        /** \brief CAN controller */
        const CanController* m_can_controller;


        /** \brief Output data in text mode */
        void textOutput(const CanData& can_data);

        /** \brief Output data in CSV mode */
        void csvOutput(const CanData& can_data);

        /** \brief Output data in binary mode */
        void binaryOutput(const CanData& can_data);
};


#endif // CANMSGSTREAMLOGGER_H
