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

#include "osal.h"

#include <time.h>


#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


/** \brief Initialize the Operating System Abstraction Layer */
bool OSAL_Init()
{
    // No specific init needed
    return true;
}

/** \brief Get the current time in timestamp format (see CanData.h) */
void OSAL_GetTimestamp(int64_t* timestamp_sec, uint32_t* timestamp_nsec)
{
    struct timespec tp = {0};
    clock_gettime(CLOCK_REALTIME, &tp);

    *timestamp_sec = static_cast<int64_t>(tp.tv_sec);
    *timestamp_nsec = static_cast<uint32_t>(tp.tv_nsec);
}

/** \brief Converts a UNIX timestamp to a broken down time structure */
void OSAL_LocalTime(const uint64_t unix_timestamp, struct tm* tm)
{
    localtime_r(reinterpret_cast<const time_t*>(&unix_timestamp), tm);
}

#ifdef __cplusplus
}
#endif // __cplusplus
