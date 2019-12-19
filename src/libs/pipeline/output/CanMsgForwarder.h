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

#ifndef CANMSGFORWARDER_H
#define CANMSGFORWARDER_H

#include "CanPipelineStageBase.h"

class CanController;

/** \brief Utility class to forward received messages to another CAN controller */
class CanMsgForwarder : public CanPipelineStageBase
{
    public:


        /** \brief Constructor */
        CanMsgForwarder();

        /** \brief Destructor */
        virtual ~CanMsgForwarder();


        /** \brief Set the CAN controller to forward the messages to */
        void setTargetController(CanController& can_controller) { m_can_controller = &can_controller; }


    protected:

        /** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
        virtual bool processCanData(const CanData& can_data) override;


    private:

        /** \brief CAN controller to forward the messages to */
        CanController* m_can_controller;
};


#endif // CANMSGFORWARDER_H
