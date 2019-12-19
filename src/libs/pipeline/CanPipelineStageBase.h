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

#ifndef CANPIPELINESTAGEBASE_H
#define CANPIPELINESTAGEBASE_H

#include "ICanPipelineStage.h"
#include "NoCopy.h"

#include <unordered_set>

/** \brief Base class for CAN message pipeline stages implementations */
class CanPipelineStageBase : public ICanPipelineStage, public INoCopy
{
    public:

        /** \brief Constructor */
        CanPipelineStageBase(const bool is_output_stage)
        : m_is_output_stage(is_output_stage)
        , m_name("")
        , m_is_enabled(true)
        , m_childs()
        {}

        /** \brief Destructor */
        virtual ~CanPipelineStageBase()
        {}


        /** \brief Get the stage's name  */
        virtual const std::string& getName() const override { return m_name; }

        /** \brief Set the stage's name  */
        virtual void setName(const std::string& name) override { m_name = name; }

        /** \brief Enable the stage */
        virtual void enable() { m_is_enabled = true; }

        /** \brief Disable the stage */
        virtual void disable() { m_is_enabled = false; }

        /** \brief Retrieve stage's state */
        virtual bool isEnabled() const override { return m_is_enabled; }

        /** \brief Indicate if the stage outputs CAN mesages */
        virtual bool isOutputStage() const override { return m_is_output_stage; }

        /** \brief Add a child pipeline stage */
        virtual void addChild(ICanPipelineStage& child) { m_childs.insert(&child); }

        /** \brief Remove a child pipeline stage */
        virtual void removeChild(ICanPipelineStage& child) { m_childs.erase(&child); }



        /** \brief Called when CAN data has been received on the controller */
        virtual void canDataReceived(const CanData& can_data)
        {
            bool forward_message = m_is_output_stage;
            if (m_is_enabled)
            {
                forward_message = processCanData(can_data);
            }
            if (m_is_output_stage && forward_message)
            {
                dispatchCanData(can_data);
            }
        }


    protected:

        /** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
        virtual bool processCanData(const CanData& can_data) = 0;


        /** \brief Dispatch CAN data to the childs */
        void dispatchCanData(const CanData& can_data)
        {
            for (auto iter = m_childs.begin(); iter != m_childs.end(); ++iter)
            {
                (*iter)->canDataReceived(can_data);
            }
        }


    private:

        /** \brief Indicate if the pipeline stage outputs messages */
        const bool m_is_output_stage;

        /** \brief Name */
        std::string m_name;

        /** \brief Indicate if the stage is enabled */
        bool m_is_enabled;

        /** \brief List of childs */
        std::unordered_set<ICanPipelineStage*> m_childs;
};


#endif // CANPIPELINESTAGEBASE_H
