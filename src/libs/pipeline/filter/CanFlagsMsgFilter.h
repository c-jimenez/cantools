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

#ifndef CANFLAGSMSGFILTER_H
#define CANFLAGSMSGFILTER_H

#include "CanPipelineStageBase.h"


/** \brief CAN message filter based on a CAN message flags */
class CanFlagsMsgFilter : public CanPipelineStageBase
{
    public:

        /** \brief Can message flags */
        enum Flags
        {
            /** \brief Standard message */
            CANMSG_FLAG_STD = 1u,
            /** \brief Extended message */
            CANMSG_FLAG_EXT = 2u,
            /** \brief Remote transmission request message */
            CANMSG_FLAG_RTR = 4u
        };


        /** \brief Constructor */
        CanFlagsMsgFilter(const uint8_t acceptance_mask);

        /** \brief Destructor */
        virtual ~CanFlagsMsgFilter();



        /** \brief Add a flag to the acceptance mask */
        void add(const Flags flag) { m_acceptance_flags |= flag; }

        /** \brief Remove a flag from the acceptance mask */
        void remove(const Flags flag) { m_acceptance_flags &= ~flag; }


    protected:

        /** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
        virtual bool processCanData(const CanData& can_data) override;


    private:

        /** \brief Acceptance flags */
        uint8_t m_acceptance_flags;


};


#endif // CANFLAGSMSGFILTER_H
