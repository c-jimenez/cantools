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

#include "IxxatCanAdapterEnumerator.h"
#include "IxxatCanEnumerator.h"


/** \brief Get the list of available controllers */
bool DYN_LINK_API CanAdapterEnumerator_EnumerateControllers(void* enumerator, CanControllerInformation** controllers)
{
    IxxatCanEnumerator* ixxat_enumerator = reinterpret_cast<IxxatCanEnumerator*>(enumerator);
    return ixxat_enumerator->enumerateControllers(controllers);
}

/** \brief Instanciate a CAN controller from its enumerate name */
CanAdapterController* DYN_LINK_API CanAdapterEnumerator_CreateController(void* enumerator, const char* controller_name)
{
    IxxatCanEnumerator* ixxat_enumerator = reinterpret_cast<IxxatCanEnumerator*>(enumerator);
    return ixxat_enumerator->createController(controller_name);
}

/** \brief Retrieve the last error message */
const char* DYN_LINK_API CanAdapterEnumerator_GetErrorMsg(void* enumerator)
{
    IxxatCanEnumerator* ixxat_enumerator = reinterpret_cast<IxxatCanEnumerator*>(enumerator);
    return ixxat_enumerator->getErrorMsg();
}
