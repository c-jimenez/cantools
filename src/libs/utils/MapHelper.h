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

#ifndef MAPHELPER_H
#define MAPHELPER_H

#include <map>


/** \brief Delete all values inside a deque and clears it */
template <typename KeyType, typename ValueType>
static inline void delete_all(std::map<KeyType, ValueType*>& _map)
{
    for (auto iter = _map.cbegin(); iter != _map.cend(); ++iter)
    {
        delete iter->second;
    }
    _map.clear();
}


#endif // MAPHELPER_H
