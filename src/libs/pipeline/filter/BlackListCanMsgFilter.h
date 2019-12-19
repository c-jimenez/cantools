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

#ifndef BLACKLISTCANMSGFILTER_H
#define BLACKLISTCANMSGFILTER_H

#include "CanPipelineStageBase.h"
#include "CanMsgIdRange.h"

#include <vector>
#include <unordered_set>


/** \brief CAN message filter based on a black list */
class BlackListCanMsgFilter : public CanPipelineStageBase
{
    public:


        /** \brief Constructor */
        BlackListCanMsgFilter();

        /** \brief Destructor */
        virtual ~BlackListCanMsgFilter();


        /** \brief Add a CAN message id to the black list */
        void add(const uint32_t id, const bool extended);

        /** \brief Add a range of CAN message ids to the black list */
        void add(const CanMsgIdRange& range, const bool extended);

        /** \brief Remove a CAN message id from the black list */
        void remove(const uint32_t id, const bool extended);

        /** \brief Remove a range of CAN message ids from the black list */
        void remove(const CanMsgIdRange& range, const bool extended);



    protected:

        /** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
        virtual bool processCanData(const CanData& can_data);


    private:

        /** \brief Black list */
        typedef std::unordered_set<uint32_t> black_list_t;

        /** \brief Black list for standard messages */
        black_list_t m_black_list_std;

        /** \brief Black list for extended messages */
        black_list_t m_black_list_ext;
};


#endif // BLACKLISTCANMSGFILTER_H
