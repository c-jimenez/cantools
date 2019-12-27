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

#include "UdpCanSerializer.h"
#include "MulticastUdpSocket.h"

#include <cstring>
#include <random>
#include <chrono>
#include <limits>
using namespace std;

/** \brief Constructor */
UdpCanSerializer::UdpCanSerializer(MulticastUdpSocket& socket)
: m_socket(socket)
, m_timeout(numeric_limits<uint32_t>::max())
, m_uid(0)
{
    mt19937_64 rand_gen;
    rand_gen.seed(static_cast<uint64_t>(chrono::system_clock::to_time_t(chrono::system_clock::now())));
    m_uid = rand_gen();
}

/** \brief Constructor */
UdpCanSerializer::UdpCanSerializer(MulticastUdpSocket& socket, const uint32_t timeout)
: m_socket(socket)
, m_timeout(timeout)
, m_uid(0)
{
    mt19937_64 rand_gen;
    rand_gen.seed(static_cast<uint64_t>(chrono::system_clock::to_time_t(chrono::system_clock::now())));
    m_uid = rand_gen();
}

/** \brief Destructor */
UdpCanSerializer::~UdpCanSerializer()
{}


/** \brief Write a CAN message to the socket */
bool UdpCanSerializer::write(const CanMsg& can_msg)
{
    uint8_t buffer[32u] = { 0 };
    uint8_t buffer_index = 0;
    bool ret = write(m_uid, &buffer[buffer_index], buffer_index, sizeof(buffer));
    if (ret)
    {
        uint16_t type = CAN_MSG;
        ret = write(type, &buffer[buffer_index], buffer_index, sizeof(buffer));
        if (ret)
        {
            ret = write(can_msg.id, &buffer[buffer_index], buffer_index, sizeof(buffer));
            if (ret)
            {
                uint8_t flags = 0;
                if (can_msg.extended)
                {
                    flags |= FLAG_EXT;
                }
                if (can_msg.rtr)
                {
                    flags |= FLAG_RTR;
                }
                ret = write(flags, &buffer[buffer_index], buffer_index, sizeof(buffer));
                if (ret)
                {
                    ret = write(can_msg.size, &buffer[buffer_index], buffer_index, sizeof(buffer));
                    if (ret && (can_msg.size > 0))
                    {
                        ret = (can_msg.size <= CAN_MSG_MAX_LEN);
                        if (ret)
                        {
                            ret = write(can_msg.data, can_msg.size, &buffer[buffer_index], buffer_index, sizeof(buffer));
                        }
                        if (ret)
                        {
                            ret = m_socket.send(buffer, buffer_index);
                        }
                    }
                }
            }
        }
    }
    return ret;
}

/** \brief Read a CAN message from the socket */
bool UdpCanSerializer::read(UdpMsg& udp_msg, UdpMsgType& msg_type)
{
    uint8_t buffer[32u] = { 0 };
    uint8_t buffer_index = 0;
    size_t received = 0;
    bool ret = m_socket.recv(buffer, sizeof(buffer), received, m_timeout);
    if (ret)
    {
        uint64_t id = 0;
        ret = read(id, &buffer[buffer_index], buffer_index, received);
        ret = ret && (id != m_uid);
        if (ret)
        {
            uint16_t type = 0;
            ret = read(type, &buffer[buffer_index], buffer_index, received);
            if (ret)
            {
                ret = (type == CAN_MSG);
                if (ret)
                {
                    ret = read(udp_msg.can_msg.id, &buffer[buffer_index], buffer_index, received);
                    if (ret)
                    {
                        uint8_t flags = 0;
                        ret = read(flags, &buffer[buffer_index], buffer_index, received);
                        if (ret)
                        {
                            udp_msg.can_msg.extended = ((flags & FLAG_EXT) != 0);
                            udp_msg.can_msg.rtr = ((flags & FLAG_RTR) != 0);

                            ret = read(udp_msg.can_msg.size, &buffer[buffer_index], buffer_index, received);
                            if (ret && (udp_msg.can_msg.size > 0))
                            {
                                ret = (udp_msg.can_msg.size <= CAN_MSG_MAX_LEN);
                                if (ret)
                                {
                                    ret = read(udp_msg.can_msg.data, udp_msg.can_msg.size, &buffer[buffer_index], buffer_index, received);
                                }
                            }
                        }
                    }
                }
            }
            if (ret)
            {
                msg_type = CAN_MSG;
            }
            else
            {
                msg_type = CAN_ERR_FRAME;
                ret = true;
            }
        }
    }
    return ret;
}

/** \brief Write an 8 bits unsigned value to a buffer */
bool UdpCanSerializer::write(const uint8_t& value, uint8_t* buffer, uint8_t& index, const size_t max_size)
{
    return write(&value, sizeof(value), buffer, index, max_size);
}

/** \brief Write a 16 bits unsigned value to a buffer */
bool UdpCanSerializer::write(const uint16_t& value, uint8_t* buffer, uint8_t& index, const size_t max_size)
{
    const uint8_t data[] = {
                            static_cast<uint8_t>(value & 0xFFu),
                            static_cast<uint8_t>((value >> 8u) & 0xFFu)
                           };
    return write(data, sizeof(data), buffer, index, max_size);
}

/** \brief Write a 32 bits unsigned value to a buffer */
bool UdpCanSerializer::write(const uint32_t& value, uint8_t* buffer, uint8_t& index, const size_t max_size)
{
    const uint8_t data[] = {
                            static_cast<uint8_t>(value & 0xFFu),
                            static_cast<uint8_t>((value >> 8u) & 0xFFu),
                            static_cast<uint8_t>((value >> 16u) & 0xFFu),
                            static_cast<uint8_t>((value >> 24u) & 0xFFu)
                           };
    return write(data, sizeof(data), buffer, index, max_size);
}

/** \brief Write a 64 bits unsigned value to a buffer */
bool UdpCanSerializer::write(const uint64_t& value, uint8_t* buffer, uint8_t& index, const size_t max_size)
{
    const uint8_t data[] = {
                            static_cast<uint8_t>(value & 0xFFu),
                            static_cast<uint8_t>((value >> 8u) & 0xFFu),
                            static_cast<uint8_t>((value >> 16u) & 0xFFu),
                            static_cast<uint8_t>((value >> 24u) & 0xFFu),
                            static_cast<uint8_t>((value >> 32u) & 0xFFu),
                            static_cast<uint8_t>((value >> 40u) & 0xFFu),
                            static_cast<uint8_t>((value >> 48u) & 0xFFu),
                            static_cast<uint8_t>((value >> 56u) & 0xFFu)
                           };
    return write(data, sizeof(data), buffer, index, max_size);
}

/** \brief Write a data buffer to a buffer */
bool UdpCanSerializer::write(const uint8_t* data_buffer, const size_t size, uint8_t* buffer, uint8_t& index, const size_t max_size)
{
    bool ret = false;

    if ((index + size) <= max_size)
    {
        memcpy(buffer, data_buffer, size);
        index += static_cast<uint8_t>(size);
        ret = true;
    }

    return ret;
}

/** \brief Read an 8 bits unsigned value from a buffer */
bool UdpCanSerializer::read(uint8_t& value, const uint8_t* buffer, uint8_t& index, const size_t max_size)
{
    return read(&value, sizeof(value), buffer, index, max_size);
}

/** \brief Read a 16 bits unsigned value from a buffer */
bool UdpCanSerializer::read(uint16_t& value, const uint8_t* buffer, uint8_t& index, const size_t max_size)
{
    uint8_t data[2u] = { 0 };
    bool ret = read(data, sizeof(data), buffer, index, max_size);
    value = data[0] + (data[1u] << 8u);
    return ret;
}

/** \brief Read a 32 bits unsigned value from a buffer */
bool UdpCanSerializer::read(uint32_t& value, const uint8_t* buffer, uint8_t& index, const size_t max_size)
{
    uint8_t data[4u] = { 0 };
    bool ret = read(data, sizeof(data), buffer, index, max_size);
    value = data[0] + (data[1u] << 8u) + (data[2u] << 16u) + (data[3u] << 24u);
    return ret;
}

/** \brief Read a 64 bits unsigned value from a buffer */
bool UdpCanSerializer::read(uint64_t& value, const uint8_t* buffer, uint8_t& index, const size_t max_size)
{
    uint8_t data[8u] = { 0 };
    bool ret = read(data, sizeof(data), buffer, index, max_size);
    value = data[0] + (data[1u] << 8u) + (data[2u] << 16u) + (data[3u] << 24u) + 
            (static_cast<uint64_t>(data[4u]) << 32u) + (static_cast<uint64_t>(data[5u]) << 40u) + (static_cast<uint64_t>(data[6u]) << 48u) + (static_cast<uint64_t>(data[7u]) << 56u);
    return ret;
}

/** \brief Read a buffer from a buffer */
bool UdpCanSerializer::read(uint8_t* data_buffer, const size_t size, const uint8_t* buffer, uint8_t& index, const size_t max_size)
{
    bool ret = false;

    if ((index + size) <= max_size)
    {
        memcpy(data_buffer, buffer, size);
        index += static_cast<uint8_t>(size);
        ret = true;
    }

    return ret;
}
