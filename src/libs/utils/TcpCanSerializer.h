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

#ifndef TCPCANSERIALIZER_H
#define TCPCANSERIALIZER_H

#include "CanData.h"
#include "NoCopy.h"

#include <cstdint>

class TcpClient;


/** \brief Serialier for CAN on TCP */
class TcpCanSerializer : public INoCopy
{
    public:


        /** \brief Constructor */
        TcpCanSerializer(TcpClient& tcp_client, const uint32_t timeout);

        /** \brief Destructor */
        virtual ~TcpCanSerializer();


        /** \brief Commands */
        enum Commands
        {
            /** \brief Start controller */
            CMD_START = 0xB007u,
            /** \brief Stop controller */
            CMD_STOP = 0xDEADu,
            /** \brief CAN message */
            CMD_MSG = 0xF00Du,
            /** \brief Error frame */
            CMD_ERR_FRAME = 0xBAADu,
        };

        /** \brief Flags */
        enum Flags
        {
            /** \brief Extended */
            FLAG_EXT = 0x01u,
            /** \brief RTR */
            FLAG_RTR = 0x02u
        };



        /** \brief Write an 8 bits unsigned value to the TCP client */
        bool write(const uint8_t& value);

        /** \brief Write a 16 bits unsigned value to the TCP client */
        bool write(const uint16_t& value);

        /** \brief Write a 32 bits unsigned value to the TCP client */
        bool write(const uint32_t& value);

        /** \brief Write a CAN message to the TCP client */
        bool write(const CanMsg& can_msg);




        /** \brief Read an 8 bits unsigned value from the TCP client */
        bool read(uint8_t& value);

        /** \brief Read a 16 bits unsigned value from the TCP client */
        bool read(uint16_t& value);

        /** \brief Read a 32 bits unsigned value from the TCP client */
        bool read(uint32_t& value);

        /** \brief Read a buffer from the TCP client */
        bool read(uint8_t* buffer, const size_t size);

        /** \brief Read a CAN message from the TCP client */
        bool read(CanMsg& can_msg);


    private:
        
        /** \brief TCP client */
        TcpClient& m_tcp_client;

        /** \brief Timeout */
        const uint32_t m_timeout;

};


#endif // TCPCANSERIALIZER_H
