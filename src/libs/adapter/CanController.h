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

#ifndef CANCONTROLLER_H
#define CANCONTROLLER_H

#include "CanAdapterPluginApi.h"
#include "NoCopy.h"

class ICanControllerListener;

/** \brief CAN controller */
class CanController : public INoCopy
{
    public:


        /** \brief Constructor */
        CanController(CanAdapterController* controller, const CanAdapterPluginFuncs* plugin_funcs);
        /** \brief Destructor */
        virtual ~CanController();


        /** \brief Open the controller */
        bool open() { return m_controller->open(m_controller->controller); }

        /** \brief Close the controller */
        bool close() { return m_controller->close(m_controller->controller); }


        /** \brief Configure the controller */
        bool configure(const CanBitrate bitrate, const uint8_t mode) { return m_controller->configure(m_controller->controller, bitrate, mode); }

        /** \brief Get the controller's capababilities */
        bool getCapabilities(CanAdapterControllerCapabilities& caps) const { return m_controller->get_capabilities(m_controller->controller, &caps); }


        /** \brief Start the controller */
        bool start(ICanControllerListener& listener);

        /** \brief Stop the controller */
        bool stop() { return m_controller->stop(m_controller->controller); }


        /** \brief Send a message through the controller */
        bool send(const CanMsg& can_msg);


        /** \brief Retrieve the last error message */
        const char* getErrorMsg() const { return m_controller->get_error_msg(m_controller->controller); }


    private:

        /** \brief CAN controller */
        CanAdapterController* m_controller;

        /** \brief Plugin functions */
        const CanAdapterPluginFuncs* m_plugin_funcs;

        /** \brief Listener */
        ICanControllerListener* m_listener;


        /** \brief Callback when CAN data has been received on the controller */
        static void DYN_LINK_API canDataReceivedCallback(void* parameter, const CanData* can_data);
};


#endif // CANCONTROLLER_H
