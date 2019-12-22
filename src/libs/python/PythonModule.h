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

#ifndef PYTHONMODULE
#define PYTHONMODULE

#ifndef PY_SSIZE_T_CLEAN
#define PY_SSIZE_T_CLEAN
#endif // PY_SSIZE_T_CLEAN
#include "Python.h"

#include <string>
#include <vector>
#include <map>

class PythonType;

/** \brief Base class for python modules implementations */
class PythonModule
{
    public:

        /** \brief Constructor */
        PythonModule(const std::string& name, const std::string& doc, const bool builtin);

        /** \brief Destructor */
        virtual ~PythonModule();


        /** \brief Add a type to the module */
        bool addType(PythonType& type);


        /** \brief Register the module to the init tab */
        bool registerInittab();

        /** \brief Initialize the module */
        bool init();

        /** \brief Release the module */
        void release();


        /** \brief Get the module's name */
        const std::string& getName() const { return m_name; }


    protected:

        /** \brief Called to register the module in the inittab */
        virtual bool onRegister() { return false; }

        /** \brief Called during initialization */
        virtual bool onInit() { return true; }


        /** \brief Initialize the module */
        bool pyInit();

        /** \brief Add a method to the module */
        bool addMethod(const PyMethodDef& method);


        /** \brief Get the module's underlying python object */
        PyObject* getPyObject() { return m_py_mod; }


        /** \brief Get the module object associated to a python module object */
        static PythonModule* getModule(PyObject* py_mod);

        /** \brief Get the module object associated to a python module object */
        template <typename PythonModuleType>
        static PythonModuleType* getModule(PyObject* py_mod)
        {
            PythonModule* python_mod = getModule(py_mod);
            return dynamic_cast<PythonModuleType*>(python_mod);
        }


    private:

        /** \brief Name */
        const std::string m_name;
        
        /** \brief Documentation */
        const std::string& m_doc;

        /** \brief Indicate if it's a built-in module */
        const bool m_builtin;

        /** \brief Module's definition */
        PyModuleDef m_mod_def;

        /** \brief Python object representing the module */
        PyObject* m_py_mod;

        /** \brief Methods of the module */
        std::vector<PyMethodDef> m_methods;

        /** \brief Types of the module */
        std::map<std::string, PythonType*> m_types;
};

#endif // PYTHONMODULE
