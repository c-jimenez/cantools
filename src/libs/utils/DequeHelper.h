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

#ifndef DEQUEHELPER_H
#define DEQUEHELPER_H

#include <deque>


/** \brief Delete all values inside a deque and clears it */
template <typename T>
static inline void delete_all(std::deque<T*>& deq)
{
    for (size_t i = 0; i < deq.size(); i++)
    {
        delete deq[i];
    }
    deq.clear();
}


#endif // DEQUEHELPER_H
