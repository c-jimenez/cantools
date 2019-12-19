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

#ifndef IDMASKCANMSGFILTER_H
#define IDMASKCANMSGFILTER_H

#include "CanPipelineStageBase.h"

#include <list>


/** \brief CAN message filter based on a mask on the message id */
class IdMaskCanMsgFilter : public CanPipelineStageBase
{
    public:


        /** \brief Constructor */
        IdMaskCanMsgFilter();

        /** \brief Destructor */
        virtual ~IdMaskCanMsgFilter();


        /** \brief Add a message id mask filter */
        void add(const uint32_t id, const uint32_t mask, const bool extended);

        /** \brief Remove a message id mask filter */
        void remove(const uint32_t id, const uint32_t mask, const bool extended);



    protected:

        /** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
        virtual bool processCanData(const CanData& can_data) override;


    private:

        /** \brief List of mask filters */
        typedef std::list<std::pair<uint32_t, uint32_t>> filter_mask_list_t;


        /** \brief Message id mask filters for standard messages */
        filter_mask_list_t m_filters_std;

        /** \brief Message id mask filters for extended messages */
        filter_mask_list_t m_filters_ext;
};


#endif // IDMASKCANMSGFILTER_H
