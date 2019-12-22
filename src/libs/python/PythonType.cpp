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

#include "PythonType.h"

#include <sstream>
using namespace std;

/** \brief Constructor */
PythonType::PythonType(const std::string& name, const std::string& doc)
: m_name(name)
, m_doc(doc)
, m_type_def()
, m_methods()
, m_members()
, m_getsetters()
{}

/** \brief Destructor */
PythonType::~PythonType()
{}

/** \brief Initialize the type */
bool PythonType::init(PyObject* py_mod)
{
    bool ret = true;

    if (py_mod != nullptr)
    {
        if ((m_methods.size() == 0) ||
            (m_methods[m_methods.size() - 1u].ml_name != nullptr))
        {
            m_methods.push_back(PyMethodDef{ 0 });
        }
        if ((m_members.size() == 0) ||
            (m_members[m_members.size() - 1u].name != nullptr))
        {
            m_members.push_back(PyMemberDef{ 0 });
        }
        if ((m_getsetters.size() == 0) ||
            (m_getsetters[m_getsetters.size() - 1u].name != nullptr))
        {
            m_getsetters.push_back(PyGetSetDef{ 0 });
        }

        newfunc new_func = nullptr;
        size_t obj_size = 0;
        getPyObjectAllocInfo(new_func, obj_size);
        
        m_type_def = PyTypeObject {
                                    PyVarObject_HEAD_INIT(NULL, 0)
                                    m_name.c_str(),
                                    static_cast<Py_ssize_t>(obj_size),
                                    0,
                                    &PythonType::pyDealloc,
                                    0,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
                                    m_doc.c_str(),
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    0,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    &m_members[0],
                                    &m_getsetters[0],
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    0,
                                    &PythonType::pyInit,
                                    nullptr,
                                    new_func
                                 };
        if (PyType_Ready(&m_type_def) >= 0)
        {
            Py_INCREF(&m_type_def);
            ret = (PyModule_AddObject(py_mod, m_name.c_str(), reinterpret_cast<PyObject*>(&m_type_def)) >= 0);
            if (!ret)
            {
                Py_DECREF(&m_type_def);
            }
        }
    }

    return ret;
}

/** \brief Release the type */
void PythonType::release()
{
    m_methods.pop_back();
    m_members.pop_back();
    m_getsetters.pop_back();
}

/** \brief Create an instance of the type */
PyObject* PythonType::create(PyObject* arg_list)
{
    return PyObject_CallObject(reinterpret_cast<PyObject*>(&m_type_def), arg_list);
}

/** \brief Add a method to the module */
bool PythonType::addMethod(const PyMethodDef& method)
{
    bool ret = false;

    if ((m_methods.size() == 0) ||
        (m_methods[m_methods.size() - 1u].ml_name != nullptr))
    {
        m_methods.push_back(method);
        ret = true;
    }

    return ret;
}

/** \brief Add a member to the type */
bool PythonType::addMember(const PyMemberDef& member)
{
    bool ret = false;

    if ((m_members.size() == 0) ||
        (m_members[m_members.size() - 1u].name != nullptr))
    {
        m_members.push_back(member);
        ret = true;
    }

    return ret;
}

/** \brief Add a getter/setter to the type */
bool PythonType::addGetSetter(const PyGetSetDef& getsetter)
{
    bool ret = false;

    if ((m_getsetters.size() == 0) ||
        (m_getsetters[m_getsetters.size() - 1u].name != nullptr))
    {
        m_getsetters.push_back(getsetter);
        ret = true;
    }

    return ret;
}

/** \brief Generic python interpreter constructor function */
PyObject* PythonType::onNew(PythonType* py_type, PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    PyObject* new_obj = type->tp_alloc(type, 0);
    if (new_obj != nullptr)
    {
        PythonObjectData* py_obj_data = reinterpret_cast<PythonObjectData*>(new_obj);
        py_obj_data->py_type = py_type;
    }
    return new_obj;
}

/** \brief Generic setter for a signed integer member */
int PythonType::intSetter(ssize_t& int_val, PyObject* new_value, const bool can_be_null, const ssize_t min_size, const ssize_t max_size)
{
    if (!can_be_null && (new_value == nullptr))
    {
        PyErr_SetString(PyExc_TypeError, "The attribute cannot be null");
        return -1;
    }
    if (!PyLong_CheckExact(new_value))
    {
        PyErr_SetString(PyExc_TypeError, "The attribute value must be a signed integer");
        return -1;
    }
    ssize_t val = PyLong_AsSsize_t(new_value);
    if (val < min_size)
    {
        stringstream error;
        error << "The attribute value must be >= " << min_size;
        PyErr_SetString(PyExc_TypeError, error.str().c_str());
        return -1;
    }
    if (val > max_size)
    {
        stringstream error;
        error << "The attribute value must be <= " << max_size;
        PyErr_SetString(PyExc_TypeError, error.str().c_str());
        return -1;
    }

    int_val = val;

    return 0;
}

/** \brief Generic setter for an unsigned integer member */
int PythonType::uintSetter(size_t& uint_val, PyObject* new_value, const bool can_be_null, const ssize_t min_size, const ssize_t max_size)
{
    if (!can_be_null && (new_value == nullptr))
    {
        PyErr_SetString(PyExc_TypeError, "The attribute cannot be null");
        return -1;
    }
    if (!PyLong_CheckExact(new_value))
    {
        PyErr_SetString(PyExc_TypeError, "The attribute value must be an unsigned integer");
        return -1;
    }
    size_t val = PyLong_AsSize_t(new_value);
    if ((val == static_cast<size_t>(-1)) && (PyErr_Occurred() != nullptr))
    {
        PyErr_SetString(PyExc_TypeError, "The attribute value must be an unsigned integer");
        return -1;
    }
    if ((min_size >= 0) && (val < static_cast<size_t>(min_size)))
    {
        stringstream error;
        error << "The attribute value must be >= " << min_size;
        PyErr_SetString(PyExc_TypeError, error.str().c_str());
        return -1;
    }
    if ((max_size >= 0) && (val > static_cast<size_t>(max_size)))
    {
        stringstream error;
        error << "The attribute value must be <= " << max_size;
        PyErr_SetString(PyExc_TypeError, error.str().c_str());
        return -1;
    }
    
    uint_val = val;

    return 0;
}

/** \brief Generic setter for a string member */
int PythonType::stringSetter(PyObject*& str_val, PyObject* new_value, const bool can_be_null)
{
    if (!can_be_null && (new_value == nullptr))
    {
        PyErr_SetString(PyExc_TypeError, "The attribute cannot be null");
        return -1;
    }
    if (!PyUnicode_Check(new_value))
    {
        PyErr_SetString(PyExc_TypeError, "The attribute value must be a string");
        return -1;
    }

    PyObject* tmp = str_val;
    Py_XINCREF(new_value);
    str_val = new_value;
    Py_XDECREF(tmp);

    return 0;
}

/** \brief Generic setter for a bytearray member */
int PythonType::bytearraySetter(PyObject*& barray_val, PyObject* new_value, const bool can_be_null, const ssize_t max_size)
{
    if (!can_be_null && (new_value == nullptr))
    {
        PyErr_SetString(PyExc_TypeError, "The attribute cannot be null");
        return -1;
    }
    if (!PyByteArray_CheckExact(new_value))
    {
        PyErr_SetString(PyExc_TypeError, "The attribute value must be a bytearray");
        return -1;
    }
    if ((max_size >= 0) && (PyByteArray_Size(new_value) > max_size))
    {
        stringstream error;
        error << "The bytearray size must not exceed " << max_size << " bytes";
        PyErr_SetString(PyExc_TypeError, error.str().c_str());
        return -1;
    }
    
    PyObject* tmp = barray_val;
    Py_XINCREF(new_value);
    barray_val = new_value;
    Py_XDECREF(tmp);

    return 0;
}


/** \brief Python interpreter destructor function callback */
void PythonType::pyDealloc(PyObject* self)
{
    PythonObjectData* py_obj_data = reinterpret_cast<PythonObjectData*>(self);
    py_obj_data->py_type->onDealloc(py_obj_data);
    Py_TYPE(self)->tp_free(self);
}

/** \brief Python interpreter initialization function callback */
int PythonType::pyInit(PyObject* self, PyObject* args, PyObject* kwds)
{
    PythonObjectData* py_obj_data = reinterpret_cast<PythonObjectData*>(self);
    return py_obj_data->py_type->onInit(py_obj_data, args, kwds);
}
