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

#include "PyCanToolsModule.h"

/** \brief Singleton */
PyCanToolsModule* PyCanToolsModule::m_singleton = nullptr;


/** \brief Get the unique instance */
PyCanToolsModule& PyCanToolsModule::getInstance()
{
    if (m_singleton == nullptr)
    {
        m_singleton = new PyCanToolsModule();
    }
    return *m_singleton;
}

/** \brief Constructor */
PyCanToolsModule::PyCanToolsModule()
: PythonModule("cantools", "Python Execution Environment for CAN tools", true)
{}

/** \brief Destructor */
PyCanToolsModule::~PyCanToolsModule()
{}

/** \brief Called to register the module in the inittab */
bool PyCanToolsModule::onRegister()
{
    return (PyImport_AppendInittab("cantools", &PyCanToolsModule::pyImportInit) == 0);
}

/** \brief Method which initialize the module on import */
PyObject* PyCanToolsModule::pyImportInit()
{
    PyObject* py_mod = nullptr;

    // Get module
    PyCanToolsModule& module = PyCanToolsModule::getInstance();

    // Generic module init
    if (module.pyInit())
    {
        py_mod = module.getPyObject();
    }

    return py_mod;
}
