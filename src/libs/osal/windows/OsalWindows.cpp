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

#include <Windows.h>


/** \brief Indicate if OSAL init has been done */
static bool osal_init_done = false;

/** \brief Performance counters frequency */
static LARGE_INTEGER pc_frequency = { 0 };

/** \brief Timestamp reference */
static int64_t timestamp_reference = 0;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus


/** \brief Initialize the Operating System Abstraction Layer */
bool OSAL_Init()
{
    bool ret = true;
    if (!osal_init_done)
    {
        // Initialize socket communication
        WORD version_requested = MAKEWORD(2, 2);;
        WSADATA wsa_data = { 0 };
        int err = WSAStartup(version_requested, &wsa_data);
        ret = (err == 0);

        // Initialize performance counter computations
        if (ret)
        {
            ret = (QueryPerformanceFrequency(&pc_frequency) != 0);
            if (ret)
            {
                time_t current_time;
                do
                {
                    current_time = time(NULL);

                    LARGE_INTEGER count = { 0 };
                    QueryPerformanceCounter(&count);

                    timestamp_reference = count.QuadPart / pc_frequency.QuadPart;
                } while (current_time != time(NULL));

                timestamp_reference = static_cast<int64_t>(current_time) - timestamp_reference;
            }
        }

        // Save init status
        osal_init_done = ret;
    }

    return ret;
}

/** \brief Get the current time in timestamp format (see CanData.h) */
void OSAL_GetTimestamp(int64_t* timestamp_sec, uint32_t* timestamp_nsec)
{
    LARGE_INTEGER count = { 0 };
    QueryPerformanceCounter(&count);

    *timestamp_sec = timestamp_reference + count.QuadPart / pc_frequency.QuadPart;
    *timestamp_nsec = static_cast<uint32_t>(((count.QuadPart % pc_frequency.QuadPart) * (1E9)) / pc_frequency.QuadPart);
}

/** \brief Converts a UNIX timestamp to a broken down time structure */
void OSAL_LocalTime(const uint64_t unix_timestamp, struct tm* tm)
{
    localtime_s(tm, reinterpret_cast<const time_t*>(&unix_timestamp));
}

#ifdef __cplusplus
}
#endif // __cplusplus
