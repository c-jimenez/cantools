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

#ifndef UDPCANCONTROLLER_H
#define UDPCANCONTROLLER_H

#include "UdpCanAdapterController.h"
#include "NoCopy.h"
#include "MulticastUdpSocket.h"
#include "UdpCanSerializer.h"

#include <string>
#include <thread>

/** \brief CAN controller implementation for CAN on multicast UDP */
class UdpCanController : public INoCopy
{
    public:


        /** \brief Constructor */
        UdpCanController(const std::string& ip, const uint16_t port);

        /** \brief Destructor */
        virtual ~UdpCanController();


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


        /** \brief IP address */
        const std::string m_ip;

        /** \brief Port */
        const uint16_t m_port;

        /** \brief Last error message */
        std::string m_last_error;

        /** \brief Capabilities */
        CanAdapterControllerCapabilities m_caps;

        /** \brief Multicast UDP socket */
        MulticastUdpSocket m_socket;

        /** \brief UDP serializer */
        UdpCanSerializer m_serializer;

        /** \brief Indicate if the Rx thread must stop */
        bool m_stop_rx_thread;

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


#endif // UDPCANCONTROLLER_H
