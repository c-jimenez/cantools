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

#include "TcpCanSerializer.h"
#include "TcpClient.h"


/** \brief Constructor */
TcpCanSerializer::TcpCanSerializer(TcpClient& tcp_client, const uint32_t timeout)
: m_tcp_client(tcp_client)
, m_timeout(timeout)
{}

/** \brief Destructor */
TcpCanSerializer::~TcpCanSerializer()
{}




/** \brief Write an 8 bits unsigned value to the TCP client */
bool TcpCanSerializer::write(const uint8_t& value)
{
    return m_tcp_client.send(&value, sizeof(value));
}

/** \brief Write a 16 bits unsigned value to the TCP client */
bool TcpCanSerializer::write(const uint16_t& value)
{
    const uint8_t data[] = {
                            static_cast<uint8_t>(value & 0xFFu),
                            static_cast<uint8_t>((value >> 8u) & 0xFFu)
                           };
    return m_tcp_client.send(data, sizeof(data));
}

/** \brief Write a 32 bits unsigned value to the TCP client */
bool TcpCanSerializer::write(const uint32_t& value)
{
    const uint8_t data[] = {
                            static_cast<uint8_t>(value & 0xFFu),
                            static_cast<uint8_t>((value >> 8u) & 0xFFu),
                            static_cast<uint8_t>((value >> 16u) & 0xFFu),
                            static_cast<uint8_t>((value >> 24u) & 0xFFu)
                           };
    return m_tcp_client.send(data, sizeof(data));
}

/** \brief Write a CAN message to the TCP client */
bool TcpCanSerializer::write(const CanMsg& can_msg)
{
    uint16_t cmd = CMD_MSG;
    bool ret = write(cmd);
    if (ret)
    {
        ret = write(can_msg.id);
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
            ret = write(flags);
            if (ret)
            {
                ret = write(can_msg.size);
                if (ret && (can_msg.size > 0))
                {
                    ret = m_tcp_client.send(can_msg.data, can_msg.size);
                }
            }
        }
    }
    return ret;
}

/** \brief Read an 8 bits unsigned value from the TCP client */
bool TcpCanSerializer::read(uint8_t& value)
{
    return read(&value, sizeof(value));
}

/** \brief Read a 16 bits unsigned value from the TCP client */
bool TcpCanSerializer::read(uint16_t& value)
{
    uint8_t data[2u] = { 0 };
    bool ret = read(data, sizeof(data));
    value = data[0] + (data[1u] << 8u);
    return ret;
}

/** \brief Read a 32 bits unsigned value from the TCP client */
bool TcpCanSerializer::read(uint32_t& value)
{
    uint8_t data[4u] = { 0 };
    bool ret = read(data, sizeof(data));
    value = data[0] + (data[1u] << 8u) + (data[2u] << 16u) + (data[3u] << 24u);
    return ret;
}

/** \brief Read a buffer from the TCP client */
bool TcpCanSerializer::read(uint8_t* buffer, const size_t size)
{
    bool ret = true;
    size_t left = size;
    size_t index = 0;
    size_t received = 0;

    while (ret && (left != 0))
    {
        ret = m_tcp_client.recv(&buffer[index], left, received, m_timeout);
        index += received;
        left -= received;
    }

    return ret;
}

/** \brief Read a CAN message from the TCP client */
bool TcpCanSerializer::read(CanMsg& can_msg)
{
    bool ret = read(can_msg.id);
    if (ret)
    {
        uint8_t flags = 0;
        ret = read(flags);
        if (ret)
        {
            can_msg.extended = ((flags & FLAG_EXT) != 0);
            can_msg.rtr = ((flags & FLAG_RTR) != 0);

            ret = read(can_msg.size);
            if (ret && (can_msg.size > 0))
            {
                ret = read(can_msg.data, can_msg.size);
            }
        }
    }
    return ret;
}
