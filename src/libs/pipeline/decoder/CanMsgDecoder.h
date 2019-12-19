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

#ifndef CANMSGDECODER_H
#define CANMSGDECODER_H

#include "CanPipelineStageBase.h"

class ICanMsgDatabase;
class CanMsgDesc;

/** \brief Interface for all listener implementations for the utility class which decodes received CAN messages */
class ICanMsgDecoderListener
{
    public:

        /** \brief Destructor */
        virtual ~ICanMsgDecoderListener() {}

        
        /** \brief Called when message has been decoded */
        virtual void canMsgDecoded(const CanMsg& can_msg, const CanMsgDesc& decoded_can_msg) = 0;
};


/** \brief Utility class to decode received messages to antoher CAN controller */
class CanMsgDecoder : public CanPipelineStageBase
{
    public:


        /** \brief Constructor */
        CanMsgDecoder();

        /** \brief Destructor */
        virtual ~CanMsgDecoder();


        /** \brief Set the CAN messages database */
        void setCanMsgDatabase(ICanMsgDatabase& can_msg_database) { m_can_msg_database = &can_msg_database; }

        /** \brief Set the listener to decoded messages */
        void setListener(ICanMsgDecoderListener& listener) { m_listener = &listener; }


    protected:

        /** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
        virtual bool processCanData(const CanData& can_data) override;


    private:

        /** \brief CAN messages database */
        ICanMsgDatabase* m_can_msg_database;

        /** \brief Listener */
        ICanMsgDecoderListener* m_listener;
};


#endif // CANMSGDECODER_H
