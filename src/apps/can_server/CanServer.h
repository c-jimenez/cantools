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

#ifndef CANSERVER_H
#define CANSERVER_H

#include "TcpServer.h"
#include "CanData.h"
#include "NoCopy.h"

#include <list>
#include <mutex>
#include <thread>


class CanClient;

/** \brief TCP server to provide a virtual CAN bus */
class CanServer : public INoCopy
{
    public:

        /** \brief Constructor */
        CanServer();

        /** \brief Destructor */
        virtual ~CanServer();


        /** \brief Start the server */
        bool start(const std::string& ip_address, const uint16_t port, const uint32_t timeout);


    private:

        /** \brief TCP server */
        TcpServer m_tcp_server;

        /** \brief List of connected clients */
        std::list<CanClient*> m_clients;

        /** \brief List of disconnected clients */
        std::vector<CanClient*> m_disconnected_clients;

        /** \brief Cleanup thread */
        std::thread* m_thread;

        /** \brief Mutex */
        std::mutex m_mutex;

        /** \brief Timeout */
        uint32_t m_timeout;



        /** \brief Called when a new client is connected to the server */
        void onNewClient(TcpClient* tcp_client);

        /** \brief Called when a client is disconnected from the server */
        void onLostClient(CanClient& can_client);

        /** \brief Called when a CAN message has been received by a client */
        void onCanMsgReceived(CanClient& can_client, const CanMsg& can_msg);

        /** \brief Cleanup thread */
        void cleanupThread();

};


#endif // CANSERVER_H
