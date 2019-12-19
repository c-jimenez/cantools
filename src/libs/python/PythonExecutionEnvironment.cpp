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

#define PY_SSIZE_T_CLEAN
#include "Python.h"


#include "PythonExecutionEnvironment.h"
#include "PythonModule.h"


/** \brief Singleton */
PythonExecutionEnvironment* PythonExecutionEnvironment::m_singleton = nullptr;


/** \brief Get the unique instance */
PythonExecutionEnvironment& PythonExecutionEnvironment::getInstance()
{
    if (m_singleton == nullptr)
    {
        m_singleton = new PythonExecutionEnvironment();
    }
    return (*m_singleton);
}

/** \brief Delete the unique instance */
void PythonExecutionEnvironment::deleteInstance()
{
    delete m_singleton;
    m_singleton = nullptr;
}


/** \brief Add a module to the execution environment */
bool PythonExecutionEnvironment::addModule(PythonModule& module)
{
    bool ret = false;

    if (m_program == nullptr)
    {
        m_modules[module.getName()] = &module;
        ret = true;
    }

    return ret;
}


/** \brief Initialize the execution environment */
bool PythonExecutionEnvironment::initialize(const std::string& prog_path)
{
    bool ret = false;

    if (m_program == nullptr)
    {
        m_program = Py_DecodeLocale(prog_path.c_str(), NULL);
        ret = (m_program != nullptr);
        if (!ret)
        {
            m_last_error = "Cannot decode program name";
        }
        else
        {
            for (auto iter = m_modules.begin(); iter != m_modules.end(); ++iter)
            {
                ret = ret && iter->second->registerInittab();
            }
            if (ret)
            {
                Py_SetProgramName(m_program);
                Py_Initialize();

                for (auto iter = m_modules.begin(); iter != m_modules.end(); ++iter)
                {
                    ret = ret && iter->second->init();
                }
                if (!ret)
                {
                    release();
                }
            }
        }
    }

    return ret;
}

/** \brief Release the execution environment */
void PythonExecutionEnvironment::release()
{
    Py_FinalizeEx();
    for (auto iter = m_modules.begin(); iter != m_modules.end(); ++iter)
    {
        iter->second->release();
    }
    if (m_program != nullptr)
    {
        PyMem_RawFree(m_program);
        m_program = nullptr;
    }
}


/** \brief Constructor */
PythonExecutionEnvironment::PythonExecutionEnvironment()
: m_last_error("")
, m_program(nullptr)
, m_modules()
{}

/** \brief Destructor */
PythonExecutionEnvironment::~PythonExecutionEnvironment()
{
    release();
}
