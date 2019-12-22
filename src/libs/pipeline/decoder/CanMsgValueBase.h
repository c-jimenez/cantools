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

#ifndef CANMSGVALUEBASE_H
#define CANMSGVALUEBASE_H

#include "ICanMsgValue.h"
#include "CanMsgDesc.h"

/** \brief Base class for CAN message values implementations */
class CanMsgValueBase : public ICanMsgValue
{
    public:


        /** \brief Constructor */
        CanMsgValueBase();

        /** \brief Destructor */
        virtual ~CanMsgValueBase();


        /** \brief Get the associated CAN message */
        virtual CanMsgDesc* getCanMsg() override { return m_can_msg; }

        /** \brief Set the associated CAN message */
        virtual void setCanMsg(CanMsgDesc& can_msg) override { m_can_msg = &can_msg; }


        /** \brief Get the value's byte position inside the CAN message */
        virtual uint8_t getBytePosition() const override { return m_byte_position; }

        /** \brief Set the value's byte position inside the CAN message */
        virtual bool setBytePosition(const uint8_t position) override;


        /** \brief Get the value's bit position inside its first byte into the CAN message */
        virtual uint8_t getBitPosition() const override { return m_bit_position; }

        /** \brief Set the value's byte position inside its first byte into the CAN message */
        virtual bool setBitPosition(const uint8_t position) override;


        /** \brief Get the value's start bit position inside inside the CAN message */
        virtual uint8_t getStartBitPosition() const override { return ((m_byte_position * 8u) + m_bit_position); }

        /** \brief Get the value's end bit position inside inside the CAN message */
        virtual uint8_t getEndBitPosition() const override { return (getStartBitPosition() + m_size_in_bits - 1u); }


        /** \brief Get the value's size in bits */
        virtual uint8_t getSizeInBits() const { return m_size_in_bits; }


    protected:

        /** \brief Set the value's size in bits */
        void setSizeInBits(const uint8_t size_in_bits);

        /** \brief Retrieve the bit mask of the value */
        uint64_t getBitMask() { return m_value_mask; }

        /** \brief Retrieve the masked value */
        void getMaskedValue(const CanMsg& can_msg, void* value);


    private:

        /** \brief Associated CAN message description */
        CanMsgDesc* m_can_msg;

        /** \brief Byte position */
        uint8_t m_byte_position;

        /** \brief Bit position */
        uint8_t m_bit_position;

        /** \brief Size in bit */
        uint8_t m_size_in_bits;

        /** \brief Bit mask of the value on the CAN message */
        uint64_t m_value_mask;
};


#endif // CANMSGVALUEBASE_H
