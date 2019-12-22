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

#include "DynLibLoader.h"

using namespace std;

/** \brief Constructor */
DynLibLoader::DynLibLoader()
: m_hlib(nullptr)
{}

/** \brief Destructor */
DynLibLoader::~DynLibLoader()
{
    unload();
}

/** \brief Loads a dynamic library */
bool DynLibLoader::load(const std::string& lib_path)
{
    bool ret = false;

    if (m_hlib == nullptr)
    {
        m_hlib = LoadLibraryA(lib_path.c_str());
        ret = (m_hlib != nullptr);
    }

    return ret;
}

/** \brief Unloads a dynamic library */
bool DynLibLoader::unload()
{
    bool ret = false;

    if (m_hlib != nullptr)
    {
        FreeLibrary(m_hlib);
        m_hlib = nullptr;
        ret = true;
    }

    return ret;
}

/** \brief Get the pointer to the specified function */
void* DynLibLoader::getFunctionPtr(const std::string& func_name)
{
    void* func_ptr = nullptr;

    if (m_hlib != nullptr)
    {
        func_ptr = reinterpret_cast<void*>(GetProcAddress(m_hlib, func_name.c_str()));
    }

    return func_ptr;
}