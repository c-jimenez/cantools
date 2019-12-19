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

#ifndef CANMSGDESC_H
#define CANMSGDESC_H

#include "CanData.h"

#include <string>
#include <vector>
#include <memory>

class ICanMsgValue;
class CanMsgSignal;

/** \brief Description of a CAN message in the message database */
class CanMsgDesc
{
    public:

        /** \brief Constructor */
        CanMsgDesc(const uint32_t id, const std::string& name, const bool extended);

        /** \brief Destructor */
        virtual ~CanMsgDesc();


        /** \brief Get the message's id */
        uint32_t getId() const { return m_id; }

        /** \brief Get the message's name */
        const std::string& getName() const { return m_name; }

        /** \brief Indicate if it's an extended message */
        bool isExtented() const { return m_extended; }


        /** \brief Add a message's value */
        bool addValue(std::shared_ptr<ICanMsgValue>& msg_value);

        /** \brief Get the message's values */
        const std::vector<std::shared_ptr<ICanMsgValue>>& getValues() const { return m_values; }


        /** \brief Add a message's signal */
        bool addSignal(std::shared_ptr<CanMsgSignal>& msg_signal);

        /** \brief Get the message's signals */
        const std::vector<std::shared_ptr<CanMsgSignal>>& getSignals() const { return m_signals; }


        /** \brief Update the signals from a CAN message */
        void updateSignals(const CanMsg& can_msg);


    private:

        /** \brief Id */
        const uint32_t m_id;

        /** \brief Name */
        const std::string m_name;

        /** \brief Extended flag */
        const bool m_extended;


        /** \brief List of the message's values */
        std::vector<std::shared_ptr<ICanMsgValue>> m_values;

        /** \brief List of the message's signals */
        std::vector<std::shared_ptr<CanMsgSignal>> m_signals;
};


#endif // CANMSGDESC_H
