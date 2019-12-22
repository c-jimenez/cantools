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

#ifndef IXXATCANCONTROLLER_H
#define IXXATCANCONTROLLER_H

#include "IxxatCanAdapterController.h"
#include "vcinpl.h"
#include "NoCopy.h"

#include <string>


/** \brief CAN controller implementation for IXXAT CAN interfaces */
class IxxatCanController : public INoCopy
{
    public:

        /** \brief Constructor */
        IxxatCanController(const VCIID& vciid, const uint16_t number);

        /** \brief Destructor */
        virtual ~IxxatCanController();


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
        bool send(const CanMsg* msg);


        /** \brief Retrieve the last error message */
        const char* getErrorMsg() const { return m_last_error.c_str(); }


    private:


        /** \brief Controller id */
        const VCIID m_vciid;

        /** \brief Controller number */
        const uint16_t m_number;

        /** \brief Last error message */
        std::string m_last_error;

        /** \brief Capabilities */
        CanAdapterControllerCapabilities m_caps;

        /** \brief Device's handle */
        HANDLE m_hdevice;

        /** \brief Controller's handle */
        HANDLE m_hcontroller;

        /** \brief Channel's handle */
        HANDLE m_hchannel;

        /** \brief Rx thread's handle */
        HANDLE m_hthread;

        /** \brief Callbacks */
        CanAdapterControllerCallbacks m_callbacks;

        /** \brief Tick resolution for the timestamp computation */
        double m_tick_res;


        /** \brief Rx thread */
        void rxThread();


        /** \brief Rx FIFO queue size */
        static const uint16_t RX_FIFO_SIZE = 1024u;
        /** \brief Tx FIFO queue size */
        static const uint16_t TX_FIFO_SIZE = 128u;

        /** \brief Rx thread creation callback */
        static DWORD WINAPI rxThreadProc(LPVOID parameter);
};


#endif // IXXATCANCONTROLLER_H
