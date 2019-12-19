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

#ifndef CANCLIENT_H
#define CANCLIENT_H

#include "TcpClient.h"
#include "CanData.h"
#include "TcpCanSerializer.h"
#include "NoCopy.h"

#include <mutex>
#include <thread>

/** \brief Client connected to the CAN server */
class CanClient : public INoCopy
{
    public:

        /** \brief Constructor */
        CanClient(TcpClient* tcp_client, const uint32_t timeout);

        /** \brief Destructor */
        ~CanClient();


        /** \brief Start the client */
        void start(std::function<void(CanClient&)> disconnected_callback,
                   std::function<void(CanClient&, const CanMsg&)> msg_received_callback);

        /** \brief Send a CAN message to the client */
        void sendMsg(const CanMsg& can_msg);


        /** \brief Get the client id */
        uint32_t id() const { return m_id;  }

        /** \brief Get the underlying TCP client */
        TcpClient* client() { return m_tcp_client; }

    private:

        /** \brief Id */
        const uint32_t m_id;

        /** \brief Underlying TCP client */
        TcpClient* m_tcp_client;

        /** \brief TCP serializer */
        TcpCanSerializer m_serializer;

        /** \brief Communication thread */
        std::thread* m_thread;

        /** \brief Mutex */
        std::mutex m_mutex;

        /** \brief Disconnected callback */
        std::function<void(CanClient&)> m_disconnected_callback;

        /** \brief Message received callback */
        std::function<void(CanClient&, const CanMsg&)> m_msg_received_callback;

        /** \brief Indicate if the client's CAN controller is started */
        bool m_controller_started;



        /** \brief Communication thread */
        void commThread();


        /** \brief Global client counter */
        static uint32_t m_id_counter;
};


#endif // CANCLIENT_H
