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

#include "CanSdoAbortCode.h"

#include <map>
#include <sstream>
using namespace std;

/** \brief SDO abort codes */
static const map<uint32_t, string> abort_codes = 
{
    { 0x05030000u, "Toggle bit not alternated." },
    { 0x05040000u, "SDO protocol timed out." },
    { 0x05040001u, "Client / server command specifier not valid or unknown." },
    { 0x05040002u, "Invalid block size(block mode only)." },
    { 0x05040003u, "Invalid sequence number(block mode only)." },
    { 0x05040004u, "CRC error(block mode only)." },
    { 0x05040005u, "Out of memory." },
    { 0x06010000u, "Unsupported access to an object." },
    { 0x06010001u, "Attempt to read a write only object." },
    { 0x06010002u, "Attempt to write a read only object." },
    { 0x06020000u, "Object does not exist in the object dictionary." },
    { 0x06040041u, "Object cannot be mapped to the PDO." },
    { 0x06040042u, "The number and length of the objects to be mapped would exceed PDO length." },
    { 0x06040043u, "General parameter incompatibility reason." },
    { 0x06040047u, "General internal incompatibility in the device." },
    { 0x06060000u, "Access failed due to an hardware error." },
    { 0x06070010u, "Data type does not match, length of service parameter does not match." },
    { 0x06070012u, "Data type does not match, length of service parameter too high." },
    { 0x06070013u, "Data type does not match, length of service parameter too low." },
    { 0x06090011u, "Sub - index does not exist." },
    { 0x06090030u, "Invalid value for parameter(download only)." },
    { 0x06090031u, "Value of parameter written too high(download only)." },
    { 0x06090032u, "Value of parameter written too low(download only)." },
    { 0x06090036u, "Maximum value is less than minimum value." },
    { 0x060A0023u, "Resource not available: SDO connection." },
    { 0x08000000u, "General error." },
    { 0x08000020u, "Data cannot be transferred or stored to the application." },
    { 0x08000021u, "Data cannot be transferred or stored to the application because of local control." },
    { 0x08000022u, "Data cannot be transferred or stored to the application because of the present device state." },
    { 0x08000023u, "Object dictionary dynamic generation fails or no object dictionary is present(e.g. object dictionary is generated from file and generation fails because of an file error)." },
    { 0x08000024u, "No data available." }
};


/** \brief Get the error string corresponding to an SDO abort CAN message */
const std::string& CANSDOABORTCODE_GetAbortString(const CanMsg& can_msg)
{
    // Compute abort code
    const uint32_t abort_code = can_msg.data[4u] + (can_msg.data[5u] << 8u) + (can_msg.data[6u] << 16u) + (can_msg.data[7u] << 24u);

    // Look for abort message
    auto iter = abort_codes.find(abort_code);
    if (iter != abort_codes.end())
    {
        return iter->second;
    }
    else
    {
        static const string unknown_code = "Unknown abort code";
        return unknown_code;
    }
}
