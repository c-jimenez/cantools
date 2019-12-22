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

#ifndef STRINGHELPER_H
#define STRINGHELPER_H

#include <string>
#include <locale>


/** \brief Convert a string to lowercase */
static inline std::string tolower(const std::string& str)
{
    std::string res;
    std::locale loc;

    for (size_t i = 0; i < str.size(); i++)
    {
        res[i] = std::tolower(str[i], loc);
    }

    return res;
}

/** \brief Convert a string to uppercase */
static inline std::string toupper(const std::string& str)
{
    std::string res;
    std::locale loc;

    for (size_t i = 0; i < str.size(); i++)
    {
        res[i] = std::toupper(str[i], loc);
    }

    return res;
}



#endif // STRINGHELPER_H
