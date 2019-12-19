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

#ifndef CANDATA_H
#define CANDATA_H

#ifdef __cplusplus

#include <cstdint>

extern "C"
{

#else // __cplusplus

#include <stdint.h>
#include <stdbool.h>

#endif // __cplusplus


/** \brief CAN data types */
typedef enum _CanDataType
{
    /** \brief CAN RX message */
    CAN_DATA_RX_MSG = 0x01010101u,
    /** \brief CAN TX message */
    CAN_DATA_TX_MSG = 0x02020202u,
    /** \brief CAN bus event */
    CAN_DATA_EVENT = 0x03030303u
} CanDataType;


/** \brief Macro to check if the CAN data is a CAN message (RX or TX) */
#define CAN_DATA_IS_CANMSG(can_data)    (((can_data).header.type == CAN_DATA_RX_MSG) || ((can_data).header.type == CAN_DATA_TX_MSG))


/** \brief CAN data header */
typedef struct _CanDataHeader
{
    /** \brief Timestamp in seconds since 1970/01/01 - 00:00:00 in UTC */
    int64_t timestamp_sec;
    /** \brief Integral number of nanoseconds to add to the timestamp in seconds */
    uint32_t timestamp_nsec;
    /** \brief Data type */
    CanDataType type;
} CanDataHeader;



/** \brief Maximum length in bytes of a CAN message */
#define  CAN_MSG_MAX_LEN    8u

/** \brief CAN message */
typedef struct _CanMsg
{
    /** \brief Header */
    CanDataHeader header;

    /** \brief Identifier */
    uint32_t id;
    /** \brief Indicate if it is an extended identifier */
    bool extended;
    /** \brief Indicate if it is a remote request */
    bool rtr;
    /** \brief Data size */
    uint8_t size;
    /** \brief Data */
    uint8_t data[CAN_MSG_MAX_LEN];
} CanMsg;



/** \brief CAN bus event types */
typedef enum _CanEventType
{
    /** \brief Bus OFF */
    CANEVT_BUS_OFF = 0u,
    /** \brief Bus ON */
    CANEVT_BUS_ON = 1u,
    /** \brief No ack */
    CANEVT_NO_ACK = 2u,
    /** \brief Error frame */
    CANEVT_ERROR = 3u,
    /** \brief Rx FIFO overrun */
    CANEVT_RX_OVR = 4u,
    /** \brief Tx FIFO overrun */
    CANEVT_TX_OVR = 5u,
    /** \brief Controller error */
    CANEVT_CONTROLLER_ERROR = 0xFFu
} CanEventType;

/** \brief CAN bus event */
typedef struct _CanEvent
{
    /** \brief Header */
    CanDataHeader header;

    /** \brief Type */
    CanEventType type;
} CanEvent;




/** \brief CAN data */
typedef union _CanData
{
    /** \brief Header */
    CanDataHeader header;

    /** \brief CAN message */
    CanMsg msg;
    /** \brief CAN bus event */
    CanEvent event;
} CanData;



#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CANDATA_H
