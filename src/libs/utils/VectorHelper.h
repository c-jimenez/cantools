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

#ifndef VECTORHELPER_H
#define VECTORHELPER_H

#include <vector>


/** \brief Look for a value in the vector from the start of the vector */
template <typename T>
static inline typename std::vector<T>::const_iterator find(const std::vector<T>& vect, const T& value)
{
    auto iter = vect.cbegin();
    while ((iter != vect.end()) && (*iter != value))
    {
        ++iter;
    }
    return iter;
}

/** \brief Look for a value in the vector from the end of the vector */
template <typename T>
static inline typename std::vector<T>::const_iterator rfind(const std::vector<T>& vect, const T& value)
{
    auto iter = vect.crbegin();
    while ((iter != vect.crend()) && (*iter != value))
    {
        --iter;
    }
    return iter;
}

/** \brief Delete all values inside a vector and clears it */
template <typename T>
static inline void delete_all(std::vector<T*>& vect)
{
    for (size_t i = 0; i < vect.size(); i++)
    {
        delete vect[i];
    }
    vect.clear();
}


#endif // VECTORHELPER_H
