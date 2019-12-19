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

#include "IxxatCanController.h"
#include "IxxatUtils.h"
#include "ICanControllerListener.h"
#include "osal.h"

#include <cmath>

using namespace std;

/** \brief Constructor */
IxxatCanController::IxxatCanController(const VCIID& vciid, const uint16_t number)
: m_vciid(vciid)
, m_number(number)
, m_last_error("")
, m_caps()
, m_hdevice(nullptr)
, m_hcontroller(nullptr)
, m_hchannel(nullptr)
, m_hthread(nullptr)
, m_callbacks()
{}

/** \brief Destructor */
IxxatCanController::~IxxatCanController()
{
    if (m_hcontroller != nullptr)
    {
        close();
    }
}


/** \brief Open the controller */
bool IxxatCanController::open()
{
    bool ret = false;

    // Initialize OSAL
    if (OSAL_Init())
    {
        // Open device
        HRESULT res = vciDeviceOpen(m_vciid, &m_hdevice);
        if (res != VCI_OK)
        {
            IxxatUtils::errorToString(res, m_last_error);
        }
        else
        {
            // Create channel
            res = canChannelOpen(m_hdevice, m_number, TRUE, &m_hchannel);
            if (res != VCI_OK)
            {
                IxxatUtils::errorToString(res, m_last_error);
            }
            else
            {
                res = canChannelInitialize(m_hchannel, RX_FIFO_SIZE, 1u, TX_FIFO_SIZE, 1u);
                if (res != VCI_OK)
                {
                    IxxatUtils::errorToString(res, m_last_error);
                }
                else
                {
                    res = canChannelActivate(m_hchannel, TRUE);
                    if (res != VCI_OK)
                    {
                        IxxatUtils::errorToString(res, m_last_error);
                    }
                    else
                    {
                        // Open controller
                        res = canControlOpen(m_hdevice, m_number, &m_hcontroller);
                        if (res != VCI_OK)
                        {
                            IxxatUtils::errorToString(res, m_last_error);
                        }
                        else
                        {
                            CANCAPABILITIES device_caps = { 0 };
                            res = canControlGetCaps(m_hcontroller, &device_caps);
                            if (res != VCI_OK)
                            {
                                IxxatUtils::errorToString(res, m_last_error);
                            }
                            else
                            {
                                // Fill capabilities
                                if ((device_caps.wBusCoupling & CAN_BUSC_HIGHSPEED) != 0)
                                {
                                    m_caps.max_bitrate = CANBR_1000K;
                                }
                                else
                                {
                                    m_caps.max_bitrate = CANBR_125K;
                                }
                                m_caps.extended = ((device_caps.dwFeatures & CAN_FEATURE_STDOREXT) != 0);
                                m_caps.bus_load = ((device_caps.dwFeatures & CAN_FEATURE_BUSLOAD) != 0);
                                m_caps.no_ack = ((device_caps.dwFeatures & CAN_FEATURE_LISTONLY) != 0);

                                m_tick_res = static_cast<double>(device_caps.dwTscDivisor) / static_cast<double>(device_caps.dwClockFreq);
                            }
                        }
                        if (res != VCI_OK)
                        {
                            canControlClose(m_hcontroller);
                            m_hcontroller = nullptr;
                        }
                    }
                }
                if (res != VCI_OK)
                {
                    canChannelClose(m_hchannel);
                    m_hchannel = nullptr;
                }
            }
        }

        ret = (res == VCI_OK);
    }

    return ret;
}

/** \brief Close the controller */
bool IxxatCanController::close()
{
    HRESULT res = VCI_E_UNEXPECTED;

    // Close all handles
    if (m_hcontroller != nullptr)
    {
        res = canControlClose(m_hcontroller);
        m_hcontroller = nullptr;
    }
    if (m_hchannel != nullptr)
    {
        res = canChannelClose(m_hchannel);
        m_hchannel = nullptr;
    }
    if (m_hdevice != nullptr)
    {
        res = vciDeviceClose(m_hdevice);
        m_hdevice = nullptr;
    }

    return (res == VCI_OK);
}

/** \brief Configure the controller */
bool IxxatCanController::configure(const CanBitrate bitrate, const uint8_t mode)
{
    HRESULT res = VCI_E_UNEXPECTED;

    if (m_hcontroller != nullptr)
    {
        // Initialize controller
        uint8_t bmode, btr0, btr1;
        switch (bitrate)
        {
            case CANBR_5K:
            {
                btr0 = CAN_BT0_5KB;
                btr1 = CAN_BT1_5KB;
                break;
            }
            case CANBR_10K:
            {
                btr0 = CAN_BT0_10KB;
                btr1 = CAN_BT1_10KB;
                break;
            }
            case CANBR_20K:
            {
                btr0 = CAN_BT0_20KB;
                btr1 = CAN_BT1_20KB;
                break;
            }
            case CANBR_50K:
            {
                btr0 = CAN_BT0_50KB;
                btr1 = CAN_BT1_50KB;
                break;
            }
            case CANBR_100K:
            {
                btr0 = CAN_BT0_100KB;
                btr1 = CAN_BT1_100KB;
                break;
            }
            case CANBR_125K:
            {
                btr0 = CAN_BT0_125KB;
                btr1 = CAN_BT1_125KB;
                break;
            }
            case CANBR_250K:
            {
                btr0 = CAN_BT0_250KB;
                btr1 = CAN_BT1_250KB;
                break;
            }
            case CANBR_500K:
            {
                btr0 = CAN_BT0_500KB;
                btr1 = CAN_BT1_500KB;
                break;
            }
            case CANBR_800K:
            {
                btr0 = CAN_BT0_800KB;
                btr1 = CAN_BT1_800KB;
                break;
            }
            case CANBR_1000K:
            {
                btr0 = CAN_BT0_1000KB;
                btr1 = CAN_BT1_1000KB;
                break;
            }
            default:
            {
                btr0 = 0;
                btr1 = 0;
                break;
            }
        }
        bmode = CAN_OPMODE_ERRFRAME;
        if ((mode & CANMODE_STD) != 0)
        {
            bmode |= CAN_OPMODE_STANDARD;
        }
        if ((mode & CANMODE_EXT) != 0)
        {
            bmode |= CAN_OPMODE_EXTENDED;
        }
        res = canControlInitialize(m_hcontroller, bmode, btr0, btr1);
        if (res != VCI_OK)
        {
            IxxatUtils::errorToString(res, m_last_error);
        }
        else
        {
            // Configure filters
            if ((mode & CANMODE_STD) != 0)
            {
                res = canControlSetAccFilter(m_hcontroller, CAN_FILTER_STD, CAN_ACC_CODE_ALL, CAN_ACC_MASK_ALL);
                if (res != VCI_OK)
                {
                    IxxatUtils::errorToString(res, m_last_error);
                }
            }
            if ((res == VCI_OK) && ((mode & CANMODE_EXT) != 0))
            {
                res = canControlSetAccFilter(m_hcontroller, CAN_FILTER_EXT, CAN_ACC_CODE_ALL, CAN_ACC_MASK_ALL);
                if (res != VCI_OK)
                {
                    IxxatUtils::errorToString(res, m_last_error);
                }
            }
        }
    }

    return (res == VCI_OK);
}

/** \brief Get the controller's capababilities */
bool IxxatCanController::getCapabilities(CanAdapterControllerCapabilities* caps) const
{
    bool ret = false;

    if ((m_hcontroller != nullptr) && (caps != nullptr))
    {
        *caps = m_caps;
        ret = true;
    }

    return ret;
}


/** \brief Start the controller */
bool IxxatCanController::start(const CanAdapterControllerCallbacks* callbacks)
{
    HRESULT res = VCI_E_INVALIDARG;

    if ((m_hcontroller != nullptr) && 
        (m_hthread == nullptr) &&
        (callbacks != nullptr))
    {
        // Start controller
        res = canControlStart(m_hcontroller, TRUE);
        if (res != VCI_OK)
        {
            IxxatUtils::errorToString(res, m_last_error);
        }
        else
        {
            // Save listener
            m_callbacks = *callbacks;

            // Start receive thread
            m_hthread = CreateThread(NULL, 0, &IxxatCanController::rxThreadProc, this, 0, NULL);
            if (m_hthread == NULL)
            {
                canControlStart(m_hcontroller, FALSE);
                res = VCI_E_UNEXPECTED;
                IxxatUtils::errorToString(res, m_last_error);
            }
        }
    }
    else
    {
        IxxatUtils::errorToString(res, m_last_error);
    }

    return (res == VCI_OK);
}

/** \brief Stop the controller */
bool IxxatCanController::stop()
{
    HRESULT res = VCI_E_UNEXPECTED;

    if (m_hcontroller != nullptr)
    {
        // Close controller
        res = canControlStart(m_hcontroller, FALSE);
        if (res != VCI_OK)
        {
            IxxatUtils::errorToString(res, m_last_error);
        }
        else
        {
            // Wait for end of rx thread
            if (m_hthread != nullptr)
            {
                WaitForSingleObject(m_hthread, INFINITE);
            }
        }
    }
    else
    {
        IxxatUtils::errorToString(res, m_last_error);
    }

    return (res == VCI_OK);
}

/** \brief Send a message through the controller */
bool IxxatCanController::send(const CanMsg* msg)
{
    HRESULT res = VCI_E_INVALIDARG;

    if ((m_hcontroller != nullptr) && 
        (m_hthread != nullptr) &&
        (msg != nullptr))
    {
        // Send message
        CANMSG can_msg = { 0 };
        can_msg.dwMsgId = msg->id;
        can_msg.uMsgInfo.Bits.type = CAN_MSGTYPE_DATA;
        can_msg.uMsgInfo.Bits.ext = msg->extended ? 1 : 0;
        can_msg.uMsgInfo.Bits.rtr = msg->rtr ? 1 : 0;
        can_msg.uMsgInfo.Bits.dlc = msg->size;
        memcpy(can_msg.abData, msg->data, msg->size);

        res = canChannelPostMessage(m_hchannel, &can_msg);
        if (res != VCI_OK)
        {
            IxxatUtils::errorToString(res, m_last_error);
            if (res == VCI_E_TXQUEUE_FULL)
            {
                CanData can_data = { 0 };
                can_data.header.type = CAN_DATA_EVENT;
                OSAL_GetTimestamp(&can_data.header.timestamp_sec, &can_data.header.timestamp_nsec);
                can_data.event.type = CANEVT_TX_OVR;
                m_callbacks.data_received(m_callbacks.parameter, &can_data);
            }
        }
    }
    else
    {
        IxxatUtils::errorToString(res, m_last_error);
    }

    return (res == VCI_OK);
}

/** \brief Rx thread */
void IxxatCanController::rxThread()
{
    HRESULT res;
    bool stop = false;
    bool start_received = false;

    int64_t start_time_sec = 0;
    uint32_t start_time_nsec = 0;
    DWORD dwtime_start = 0;

    // Wait for events from controller's channel
    res = VCI_OK;
    while (!stop && (res == VCI_OK))
    {
        res = canChannelWaitRxEvent(m_hchannel, 500u);
        if (res == VCI_E_TIMEOUT)
        {
            // Check controller state
            CANLINESTATUS status = { 0 };
            res = canControlGetStatus(m_hcontroller, &status);
        }
        else if (res == VCI_OK)
        {
            CANMSG msg = { 0 };
            while ((res = canChannelPeekMessage(m_hchannel, &msg)) == VCI_OK)
            {
                CanData can_data = { 0 };

                // Timestamp
                DWORD dwtime_rel = msg.dwTime - dwtime_start;
                double rel_time = static_cast<double>(dwtime_rel) * m_tick_res;
                double rel_time_sec = 0.;
                double rel_time_nsec = modf(rel_time, &rel_time_sec) * 1000000000.;
                can_data.header.timestamp_sec = start_time_sec + static_cast<int64_t>(rel_time_sec);
                can_data.header.timestamp_nsec = start_time_nsec + static_cast<uint32_t>(rel_time_nsec);
                if (can_data.header.timestamp_nsec >= 1000000000u)
                {
                    can_data.header.timestamp_nsec -= 1000000000u;
                    can_data.header.timestamp_sec++;
                }

                // Check event type
                if (msg.uMsgInfo.Bits.type == CAN_MSGTYPE_DATA)
                {
                    if (start_received)
                    {
                        // CAN message
                        CanMsg& can_msg = can_data.msg;
                        can_data.header.type = CAN_DATA_RX_MSG;

                        can_msg.id = msg.dwMsgId;
                        can_msg.extended = (msg.uMsgInfo.Bits.ext != 0);
                        can_msg.rtr = (msg.uMsgInfo.Bits.rtr != 0);
                        can_msg.size = msg.uMsgInfo.Bits.dlc;
                        memcpy(can_msg.data, msg.abData, can_msg.size);

                        m_callbacks.data_received(m_callbacks.parameter, &can_data);
                    }
                }
                else if (msg.uMsgInfo.Bytes.bType == CAN_MSGTYPE_INFO)
                {
                    // Information
                    if (msg.abData[0] == CAN_INFO_STOP)
                    {
                        if (start_received)
                        {
                            stop = true;
                        }
                    }
                    else if (msg.abData[0] == CAN_INFO_START)
                    {
                        start_received = true;

                        // Initialize time measurement
                        OSAL_GetTimestamp(&start_time_sec, &start_time_nsec);
                        dwtime_start = msg.dwTime;
                    }
                    else
                    {
                        // Ignore
                    }
                }
                else if (msg.uMsgInfo.Bits.type == CAN_MSGTYPE_ERROR)
                {
                    // Error, send event
                    CanEvent& can_event = can_data.event;
                    can_data.header.type = CAN_DATA_EVENT;
                    if (msg.abData[0] == CAN_ERROR_ACK)
                    {
                        can_event.type = CANEVT_NO_ACK;
                    }
                    else
                    {
                        can_event.type = CANEVT_ERROR;
                    }
                    m_callbacks.data_received(m_callbacks.parameter, &can_data);
                }
                else
                {
                    // Ignore
                }
            }
            res = VCI_OK;
        }
    }  
    if (!stop)
    {
        // Controller error
        CanData can_data = { 0 };
        can_data.header.type = CAN_DATA_EVENT;
        OSAL_GetTimestamp(&can_data.header.timestamp_sec, &can_data.header.timestamp_nsec);
        can_data.event.type = CANEVT_CONTROLLER_ERROR;
        IxxatUtils::errorToString(res, m_last_error);
        m_callbacks.data_received(m_callbacks.parameter, &can_data);
    }

    // End of thread
    m_hthread = NULL;
}

/** \brief Rx thread creation callback */
DWORD WINAPI IxxatCanController::rxThreadProc(LPVOID parameter)
{
    IxxatCanController* controller = reinterpret_cast<IxxatCanController*>(parameter);
    controller->rxThread();
    return 0;
}
