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

#ifndef CANPIPELINESUSPENDER_H
#define CANPIPELINESUSPENDER_H

#include "CanPipelineStageBase.h"

#include <mutex>
#include <vector>

/** \brief Utility class to allow to suspend CAN message reception without loosing messages in a CAN pipeline */
class CanPipelineSuspender : public CanPipelineStageBase
{
    public:


        /** \brief Constructor */
        CanPipelineSuspender();

        /** \brief Destructor */
        virtual ~CanPipelineSuspender();


        /** \brief Suspend message reception */
        void suspend();

        /** \brief Resume message reception */
        void resume();
        
        /** \brief Clear the message queue */
        void clear();


        /** \brief Indicate if the suspender is active */
        bool isSuspended() const { return m_is_suspended; }


    protected:

        /** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
        virtual bool processCanData(const CanData& can_data) override;


    private:

        /** \brief Suspended state */
        bool m_is_suspended;

        /** \brief Mutex */
        std::mutex m_mutex;

        /** \brief CAN data received while suspended */
        std::vector<CanData> m_can_data;

};


#endif // CANPIPELINESUSPENDER_H
