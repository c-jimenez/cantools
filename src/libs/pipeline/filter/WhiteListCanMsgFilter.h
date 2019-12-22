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

#ifndef WHITELISTCANMSGFILTER_H
#define WHITELISTCANMSGFILTER_H

#include "CanPipelineStageBase.h"
#include "CanMsgIdRange.h"

#include <vector>
#include <unordered_set>


/** \brief CAN message filter based on a white list */
class WhiteListCanMsgFilter : public CanPipelineStageBase
{
    public:


        /** \brief Constructor */
        WhiteListCanMsgFilter();

        /** \brief Destructor */
        virtual ~WhiteListCanMsgFilter();


        /** \brief Add a CAN message id to the white list */
        void add(const uint32_t id, const bool extended);

        /** \brief Add a range of CAN message ids to the white list */
        void add(const CanMsgIdRange& range, const bool extended);

        /** \brief Remove a CAN message id from the white list */
        void remove(const uint32_t id, const bool extended);

        /** \brief Remove a range of CAN message ids from the white list */
        void remove(const CanMsgIdRange& range, const bool extended);



    protected:

        /** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
        virtual bool processCanData(const CanData& can_data) override;


    private:

        /** \brief White list */
        typedef std::unordered_set<uint32_t> white_list_t;

        /** \brief White list for standard messages */
        white_list_t m_white_list_std;

        /** \brief White list for extended messages */
        white_list_t m_white_list_ext;
};


#endif // WHITELISTCANMSGFILTER_H
