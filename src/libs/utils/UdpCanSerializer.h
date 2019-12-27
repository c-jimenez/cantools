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

#ifndef UDPCANSERIALIZER_H
#define UDPCANSERIALIZER_H

#include "CanData.h"
#include "NoCopy.h"

#include <cstdint>
#include <cstddef>

class MulticastUdpSocket;


/** \brief Serialier for CAN on multicast UDP */
class UdpCanSerializer : public INoCopy
{
    public:


        /** \brief Constructor */
        UdpCanSerializer(MulticastUdpSocket& socket);

        /** \brief Constructor */
        UdpCanSerializer(MulticastUdpSocket& socket, const uint32_t timeout);

        /** \brief Destructor */
        virtual ~UdpCanSerializer();


        /** \brief Flags */
        enum Flags
        {
            /** \brief Extended */
            FLAG_EXT = 0x01u,
            /** \brief RTR */
            FLAG_RTR = 0x02u
        };

        /** \brief UDP message types */
        enum UdpMsgType
        {
            /** \brief CAN message */
            CAN_MSG = 0xF00Du,
            /** \brief Error frame */
            CAN_ERR_FRAME = 0xBAADu
        };

        /** \brief UDP message */
        union UdpMsg
        {
            /** \brief CAN message */
            CanMsg can_msg;
        };


        /** \brief Write a CAN message to the socket */
        bool write(const CanMsg& can_msg);

        /** \brief Read an UDP message from the socket */
        bool read(UdpMsg& udp_msg, UdpMsgType& msg_type);


    private:
        
        /** \brief Multicast UDP socket */
        MulticastUdpSocket& m_socket;

        /** \brief Timeout */
        const uint32_t m_timeout;

        /** \brief Unique-ID */
        uint64_t m_uid;


        /** \brief Write an 8 bits unsigned value to a buffer */
        bool write(const uint8_t& value, uint8_t* buffer, uint8_t& index, const size_t max_size);

        /** \brief Write a 16 bits unsigned value to a buffer */
        bool write(const uint16_t& value, uint8_t* buffer, uint8_t& index, const size_t max_size);

        /** \brief Write a 32 bits unsigned value to a buffer */
        bool write(const uint32_t& value, uint8_t* buffer, uint8_t& index, const size_t max_size);

        /** \brief Write a 64 bits unsigned value to a buffer */
        bool write(const uint64_t& value, uint8_t* buffer, uint8_t& index, const size_t max_size);

        /** \brief Write a data buffer to a buffer */
        bool write(const uint8_t* data_buffer, const size_t size, uint8_t* buffer, uint8_t& index, const size_t max_size);



        /** \brief Read an 8 bits unsigned value from a buffer */
        bool read(uint8_t& value, const uint8_t* buffer, uint8_t& index, const size_t max_size);

        /** \brief Read a 16 bits unsigned value from a buffer */
        bool read(uint16_t& value, const uint8_t* buffer, uint8_t& index, const size_t max_size);

        /** \brief Read a 32 bits unsigned value from a buffer */
        bool read(uint32_t& value, const uint8_t* buffer, uint8_t& index, const size_t max_size);

        /** \brief Read a 64 bits unsigned value from a buffer */
        bool read(uint64_t& value, const uint8_t* buffer, uint8_t& index, const size_t max_size);

        /** \brief Read a data buffer from a buffer */
        bool read(uint8_t* data_buffer, const size_t size, const uint8_t* buffer, uint8_t& index, const size_t max_size);
};


#endif // UDPCANSERIALIZER_H
