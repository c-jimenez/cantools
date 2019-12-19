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

#include "IxxatUtils.h"

#include <sstream>
using namespace std;

/** \brief Get the error message corresponding to an error code */
void IxxatUtils::errorToString(const HRESULT error_code, std::string& error_msg)
{
    char buffer[256u] = { 0 };
    vciFormatErrorA(error_code, buffer, sizeof(buffer) - 1u);
    error_msg = buffer;
}

/** \brief Build the name of the device */
void IxxatUtils::buildDeviceName(const VCIDEVICEINFO& device_info, const uint16_t controller_id, std::string& name)
{
    stringstream name_stream;
    name_stream << device_info.UniqueHardwareId.AsChar << "-" << (int)controller_id;
    name = name_stream.str();
}