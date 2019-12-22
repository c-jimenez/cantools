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

#ifndef CANSDOABORTCODE_H
#define CANSDOABORTCODE_H

#include "CanData.h"

#include <string>


/** \brief Get the error string corresponding to an SDO abort CAN message */
const std::string& CANSDOABORTCODE_GetAbortString(const CanMsg& can_msg);


#endif // CANSDOABORTCODE_H
