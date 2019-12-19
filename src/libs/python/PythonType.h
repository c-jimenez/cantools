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

#ifndef PYTHONTYPE_H
#define PYTHONTYPE_H  

#ifndef PY_SSIZE_T_CLEAN
#define PY_SSIZE_T_CLEAN
#endif // PY_SSIZE_T_CLEAN
#include "Python.h"
#include "structmember.h"

#include <string>
#include <vector>
#include <limits>

/** \brief Base class for C++ classes representing a python type */
class PythonType
{
    public:

        /** \brief Constructor */
        PythonType(const std::string& name, const std::string& doc);

        /** \brief Destructor */
        virtual ~PythonType();


        /** \brief Initialize the type */
        bool init(PyObject* py_mod);

        /** \brief Release the type */
        void release();


        /** \brief Get the type's name */
        const std::string& getName() const { return m_name; }


        /** \brief Create an instance of the type */
        PyObject* create(PyObject* arg_list);


    protected:

        /** \brief Base struct for storing python object data */
        struct PythonObjectData
        {
            PyObject_HEAD
            PythonType* py_type;
        };

        /** \brief Get the python type object */
        PyTypeObject* getType() { return &m_type_def; }

        /** \brief Add a method to the module */
        bool addMethod(const PyMethodDef& method);

        /** \brief Add a member to the type */
        bool addMember(const PyMemberDef& member);

        /** \brief Add a getter/setter to the type */
        bool addGetSetter(const PyGetSetDef& getsetter);
        

        /** \brief Generic python interpreter constructor function */
        PyObject* onNew(PythonType* py_type, PyTypeObject* type, PyObject* args, PyObject* kwds);


        /** \brief Generic setter for a signed integer member */
        static int intSetter(ssize_t& int_val, PyObject* new_value, const bool can_be_null, const ssize_t min_size = std::numeric_limits<ssize_t>::min(), const ssize_t max_size = std::numeric_limits<ssize_t>::max());

        /** \brief Generic setter for an unsigned integer member */
        static int uintSetter(size_t& uint_val, PyObject* new_value, const bool can_be_null, const ssize_t min_size = -1, const ssize_t max_size = -1);

        /** \brief Generic setter for a string member */
        static int stringSetter(PyObject*& str_val, PyObject* new_value, const bool can_be_null);

        /** \brief Generic setter for a bytearray member */
        static int bytearraySetter(PyObject*& barray_val, PyObject* new_value, const bool can_be_null, const ssize_t max_size);


        /** \brief Get the object allocation information */
        virtual void getPyObjectAllocInfo(newfunc& new_func, size_t& size) = 0;

        /** \brief Python interpreter destructor function callback */
        virtual void onDealloc(PythonObjectData* self) = 0;

        /** \brief Python interpreter initialization function callback */
        virtual int onInit(PythonObjectData* self, PyObject* args, PyObject* kwds) = 0;


    private:

        /** \brief Name */
        const std::string m_name;

        /** \brief Documentation */
        const std::string& m_doc;

        /** \brief Type's definition */
        PyTypeObject m_type_def;

        /** \brief Methods of the module */
        std::vector<PyMethodDef> m_methods;

        /** \brief Members of the type */
        std::vector<PyMemberDef> m_members;

        /** \brief Getters/setters of the type */
        std::vector<PyGetSetDef> m_getsetters;


        /** \brief Python interpreter destructor function callback */
        static void pyDealloc(PyObject* self);

        /** \brief Python interpreter initialization function callback */
        static int pyInit(PyObject* self, PyObject* args, PyObject* kwds);
};


#endif // PYTHONTYPE_H
