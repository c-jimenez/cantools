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

#include "PythonModule.h"
#include "PythonType.h"

/** \brief Constructor */
PythonModule::PythonModule(const std::string& name, const std::string& doc, const bool builtin)
: m_name(name)
, m_doc(doc)
, m_builtin(builtin)
, m_mod_def()
, m_py_mod(nullptr)
, m_methods()
, m_types()
{}

/** \brief Destructor */
PythonModule::~PythonModule()
{}

/** \brief Add a type to the module */
bool PythonModule::addType(PythonType& type)
{
    bool ret = false;

    if (m_py_mod == nullptr)
    {
        m_types[type.getName()] = &type;
    }

    return ret;
}

/** \brief Register the module to the init tab */
bool PythonModule::registerInittab()
{
    bool ret = true;

    // Only needed for built-in modules
    if (m_builtin)
    {
        ret = onRegister();
    }

    return ret;
}

/** \brief Initialize the module */
bool PythonModule::init()
{
    bool ret = true;

    // Not needed for built-in modules
    if (!m_builtin)
    {
        ret = pyInit();
    }

    return ret;
}

/** \brief Initialize the module */
bool PythonModule::pyInit()
{
    bool ret = false;

    if (m_py_mod == nullptr)
    {
        // Add empty method at the end of the list
        if ((m_methods.size() == 0) ||
            (m_methods[m_methods.size() - 1].ml_name != nullptr))
        {
            m_methods.push_back(PyMethodDef{ 0 });
        }

        // Initialize module's definition
        m_mod_def = PyModuleDef{
                                    PyModuleDef_HEAD_INIT,
                                    m_name.c_str(),
                                    m_doc.c_str() ,
                                    sizeof(PythonModule**),
                                    &m_methods[0]
                                };

        m_py_mod = PyModule_Create(&m_mod_def);

        ret = (m_py_mod != nullptr);
        if (ret)
        {
            // Save reference to ourself
            PythonModule** pself = reinterpret_cast<PythonModule**>(PyModule_GetState(m_py_mod));
            *pself = this;

            // Module specific init
            ret = onInit();
            if (ret)
            {
                // Types init
                for (auto iter = m_types.begin(); ret && (iter != m_types.end()); ++iter)
                {
                    ret = iter->second->init(m_py_mod);
                }
            }
        }
        if (!ret)
        {
            Py_DECREF(m_py_mod);
        }
    }

    return ret;
}

/** \brief Release the module */
void PythonModule::release()
{
    if (m_py_mod != nullptr)
    {
        m_py_mod = nullptr;
        m_methods.pop_back();
        for (auto iter = m_types.begin(); iter != m_types.end(); ++iter)
        {
            iter->second->release();
        }
    }
}

/** \brief Add a method to the module */
bool PythonModule::addMethod(const PyMethodDef& method)
{
    bool ret = false;

    if (m_py_mod == nullptr)
    {
        m_methods.push_back(method);
        ret = true;
    }

    return ret;
}

/** \brief Get the module object associated to a python module object */
PythonModule* PythonModule::getModule(PyObject* py_mod)
{
    return *reinterpret_cast<PythonModule**>(PyModule_GetState(py_mod));
}
