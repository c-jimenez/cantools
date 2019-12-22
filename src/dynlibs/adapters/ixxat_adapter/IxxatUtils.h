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

#ifndef IXXATUTILS_H
#define IXXATUTILS_H

#include "vcinpl.h"

#include <string>

/** \brief Utilities function for IXXAT CAN interfaces */
class IxxatUtils
{
    public:


        /** \brief Get the error message corresponding to an error code */
        static void errorToString(const HRESULT error_code, std::string& error_msg);

        /** \brief Build the name of the device */
        static void buildDeviceName(const VCIDEVICEINFO& device_info, const uint16_t controller_id, std::string& name);

};


#endif // IXXATUTILS_H
