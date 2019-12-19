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

#ifndef CANSDOREADER_H
#define CANSDOREADER_H

#include "CanPipelineStageBase.h"

#include <cstdint>
#include <string>

class CanController;

/** \brief Handles a single SDO read operation */
class CanSdoReader : public CanPipelineStageBase
{
    public:

        /** \brief Constructor */
        CanSdoReader(CanController& controller);

        /** \brief Destructor */
        virtual ~CanSdoReader();


        /** \brief Start the read operation */
        bool read(const uint16_t client_sdo, const uint16_t server_sdo, const uint16_t obd_index, const uint8_t obd_subindex);

        /** \brief Get the size of the received data */
        uint32_t getSize() const { return m_data_size; }

        /** \brief Get the received data buffer */
        const uint8_t* getData() const { return m_data; }

        /** \brief Retrieve the last error message */
        const char* getErrorMsg() const { return m_last_error.c_str(); }


    protected:

        /** \brief Process received CAN data, return true if CAN data must be forwarded to childs */
        virtual bool processCanData(const CanData& can_data) override;


    private:

        /** \brief CAN controller */
        CanController& m_controller;

        /** \brief Client SDO */
        uint16_t m_client_sdo;
        /** \brief Server SDO */
        uint16_t m_server_sdo;
        /** \brief OBD index */
        uint16_t m_obd_index;
        /** \brief OBD subindex */
        uint8_t m_obd_subindex;

        /** \brief Indicate if SDO transfer has started */
        bool m_started;
        /** \brief SDO segment number */
        uint32_t m_segment_number;
        /** \brief Current index in the received data buffer */
        uint32_t m_data_index;
        /** \brief Received data size */
        uint32_t m_data_size;
        /** \brief Received data buffer */
        uint8_t m_data[4096u];
        /** \brief Indicate if the SDO transfer is complete */
        volatile bool m_transfer_complete;
        /** \brief Indicate if the SDO transfer failed */
        volatile bool m_transfer_error;
        /** \brief Current timeout value */
        volatile uint32_t m_timeout;

        /** \brief Last error message */
        std::string m_last_error;

};


#endif // CANSDOREADER_H
