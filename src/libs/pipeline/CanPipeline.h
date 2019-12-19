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

#ifndef CANPIPELINE_H
#define CANPIPELINE_H


#include "ICanControllerListener.h"
#include "NoCopy.h"
#include "MailBox.h"
#include "CanPipelineSuspender.h"

#include <string>
#include <thread>

class CanController;
class CanPipelineSuspender;

/** \brief CAN message processing pipeline */
class CanPipeline : public ICanControllerListener, public INoCopy
{
    public:


        /** \brief Constructor */
        CanPipeline(const std::string& name);
        /** \brief Destructor */
        virtual ~CanPipeline();


        /** \brief Start the pipeline with the specified CAN controller */
        bool start(CanController& controller);

        /** \brief Suspend the pipeline processing */
        bool suspend();

        /** \brief Resume the pipeline processing */
        bool resume();

        /** \brief Stop the pipeline */
        bool stop();


        /** \brief Send a message through the CAN controller of the pipeline */
        bool send(const CanMsg& can_msg);


        /** \brief Add a child pipeline stage */
        void addChild(ICanPipelineStage& child);

        /** \brief Remove a child pipeline stage */
        void removeChild(ICanPipelineStage& child);


        /** \brief Retrieve the last error message */
        const std::string& getErrorMsg() const { return m_last_error; }
        

        /** \brief Called when CAN data has been received on the controller */
        virtual void canDataReceived(const CanData& can_data) override;


    private:

        /** \brief Name */
        const std::string& m_name;

        /** \brief CAN controller */
        CanController* m_controller;

        /** \brief Last error message */
        std::string m_last_error;

        /** \brief Reception thread */
        std::thread* m_rx_thread;

        /** \brief Rx mailbox */
        MailBox<CanData> m_rx_mailbox;

        /** \brief Pipeline suspender */
        CanPipelineSuspender m_suspender;


        /** \brief Rx thread */
        void rxThread();
};


#endif // CANPIPELINE_H
