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

#ifndef CANBITRATE_H
#define CANBITRATE_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

/** \brief CAN Bitrates */
typedef enum _CanBitrate
{
    /** \brief 5 kbit/s */
    CANBR_5K = 5u,
    /** \brief 10 kbit/s */
    CANBR_10K = 10u,
    /** \brief 20 kbit/s */
    CANBR_20K = 20u,
    /** \brief 50 kbit/s */
    CANBR_50K = 50u,
    /** \brief 100 kbit/s */
    CANBR_100K = 100u,
    /** \brief 125 kbit/s */
    CANBR_125K = 125u,
    /** \brief 250 kbit/s */
    CANBR_250K = 250u,
    /** \brief 500 kbit/s */
    CANBR_500K = 500u,
    /** \brief 800 kbit/s */
    CANBR_800K = 800u,
    /** \brief 1000 kbit/s */
    CANBR_1000K = 1000u
} CanBitrate;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CANBITRATE_H
