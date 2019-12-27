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

#ifndef DYNLIBLOADER_H
#define DYNLIBLOADER_H

#include "NoCopy.h"

#include <string>


/** \brief Utility class to load a dynamic library */
class DynLibLoader : public INoCopy
{
    public:

        /** \brief Constructor */
        DynLibLoader();
        /** \brief Destructor */
        virtual ~DynLibLoader();


        /** \brief Loads a dynamic library */
        bool load(const std::string& lib_path);

        /** \brief Unloads a dynamic library */
        bool unload();

        /** \brief Get the pointer to the specified function */
        void* getFunctionPtr(const std::string& func_name);


        /** \brief Extension for the dynamic libraries */
        static constexpr const char* const DYN_LIB_EXTENSION = ".so";

    private:


        /** \brief Handle to the library */
        void* m_hlib;
};


#endif // DYNLIBLOADER_H
