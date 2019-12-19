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

#ifndef CANMSGIDRANGE_H
#define CANMSGIDRANGE_H

#include <cstdint>


/** \brief Represent a range of CAN message ids */
class CanMsgIdRange
{
    public:

        /** \brief Create a CAN message id range object */
        static CanMsgIdRange create(const uint32_t min_id, const uint32_t max_id)
        {
            if (min_id < max_id)
            {
                return CanMsgIdRange(min_id, max_id);
            }
            else
            {
                return CanMsgIdRange(max_id, min_id);
            }
        }

        /** \brief Get the minimum CAN id in the range */
        uint32_t minId() const { return m_min_id; }

        /** \brief Get the maximum CAN id in the range */
        uint32_t maxId() const { return m_max_id; }

        /** \brief Check if a CAN id is inside the range */
        bool contains(const uint32_t id) { return ((id >= m_min_id) && (id <= m_max_id)); }


    private:

        /** \brief Constructor */
        CanMsgIdRange(const uint32_t min_id, const uint32_t max_id)
        : m_min_id(min_id)
        , m_max_id(max_id)
        {}


        /** \brief Minimum CAN id in the range */
        const uint32_t m_min_id;

        /** \brief Maximum CAN id in the range */
        const uint32_t m_max_id;
};


#endif // CANMSGIDRANGE_H
