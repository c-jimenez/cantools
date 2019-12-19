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

#ifndef TCPCANCONTROLLER_H
#define TCPCANCONTROLLER_H

#include "TcpCanAdapterController.h"
#include "TcpClient.h"
#include "TcpCanSerializer.h"
#include "NoCopy.h"

#include <string>
#include <thread>

/** \brief CAN controller implementation for CAN on TCP */
class TcpCanController : public INoCopy
{
    public:


        /** \brief Constructor */
        TcpCanController(const std::string& server, const uint16_t port, const uint32_t timeout);

        /** \brief Destructor */
        virtual ~TcpCanController();


        /** \brief Open the controller */
        bool open();

        /** \brief Close the controller */
        bool close();


        /** \brief Configure the controller */
        bool configure(const CanBitrate bitrate, const uint8_t mode);

        /** \brief Get the controller's capababilities */
        bool getCapabilities(CanAdapterControllerCapabilities* caps) const;


        /** \brief Start the controller */
        bool start(const CanAdapterControllerCallbacks* callbacks);

        /** \brief Stop the controller */
        bool stop();


        /** \brief Send a message through the controller */
        bool send(const CanMsg* can_msg);


        /** \brief Retrieve the last error message */
        const char* getErrorMsg() const { return m_last_error.c_str(); }


    private:


        /** \brief Server address */
        const std::string m_server;

        /** \brief Port */
        const uint16_t m_port;

        /** \brief Communication timeout */
        const uint32_t m_timeout;

        /** \brief Last error message */
        std::string m_last_error;

        /** \brief Capabilities */
        CanAdapterControllerCapabilities m_caps;

        /** \brief TCP client */
        TcpClient m_tcp_client;

        /** \brief TCP serializer */
        TcpCanSerializer m_serializer;

        /** \brief Rx thread */
        std::thread* m_rx_thread;

        /** \brief Callbacks */
        CanAdapterControllerCallbacks m_callbacks;

        /** \brief Indicate if the controer is opened */
        bool m_opened;

        /** \brief Indicate if the controer is started */
        bool m_started;


        /** \brief Rx thread */
        void rxThread();
};


#endif // TCPCANCONTROLLER_H
