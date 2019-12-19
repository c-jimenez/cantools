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

#include "PyCanMsgType.h"

/** \brief Singleton */
PyCanMsgType* PyCanMsgType::m_singleton = nullptr;


/** \brief Get the unique instance */
PyCanMsgType& PyCanMsgType::getInstance()
{
    if (m_singleton == nullptr)
    {
        m_singleton = new PyCanMsgType();
    }
    return *m_singleton;
}

/** \brief Constructor */
PyCanMsgType::PyCanMsgType()
: PythonType("CanMsg", "CAN message")
{
    addMember(PyMemberDef{ "timestamp_sec", T_LONGLONG, offsetof(PyCanMsgObjectData, timestamp_sec), 0, "Timestamp in seconds since 1970/01/01 - 00:00:00 in UTC" });
    addMember(PyMemberDef{ "timestamp_nsec", T_ULONG, offsetof(PyCanMsgObjectData, timestamp_nsec), 0, "Integral number of nanoseconds to add to the timestamp in seconds" });
    addMember(PyMemberDef{ "id", T_ULONG, offsetof(PyCanMsgObjectData, id), 0, "Identifier" });
    addMember(PyMemberDef{ "extended", T_BOOL, offsetof(PyCanMsgObjectData, extended), 0, "Indicate if it is an extended identifier" });
    addMember(PyMemberDef{ "rtr", T_BOOL, offsetof(PyCanMsgObjectData, rtr), 0, "Indicate if it is a remote request " });

    addGetSetter(PyGetSetDef{ "size", (getter)&PyCanMsgType::getSize, (setter)&PyCanMsgType::setSize, "Data size", nullptr });
    addGetSetter(PyGetSetDef{ "data", (getter)&PyCanMsgType::getData, (setter)&PyCanMsgType::setData, "Data as a bytearray", nullptr });
}

/** \brief Destructor */
PyCanMsgType::~PyCanMsgType()
{}

/** \brief Create a python CAN message */
PyObject* PyCanMsgType::fromCanMsg(const CanMsg& can_msg)
{
    PyObject* data = PyByteArray_FromStringAndSize(reinterpret_cast<const char*>(can_msg.data), can_msg.size);
    PyObject* params = Py_BuildValue("LkkccO", can_msg.header.timestamp_sec, can_msg.header.timestamp_nsec, can_msg.id, can_msg.extended, can_msg.rtr, data);
    PyObject* py_can_msg = create(params);
    Py_DECREF(params);
    Py_DECREF(data);
    return py_can_msg;
}

/** \brief Convert as a CAN message */
bool PyCanMsgType::toCanMsg(const PyObject* py_can_msg, CanMsg& can_msg)
{
    bool ret = false;

    // Check types
    if ((py_can_msg != nullptr) &&
        (PyType_IsSubtype(py_can_msg->ob_type, getType())))
    {
        // Copy data
        const PyCanMsgObjectData* can_msg_data = reinterpret_cast<const PyCanMsgObjectData*>(py_can_msg);
        
        can_msg.header.timestamp_sec = can_msg_data->timestamp_sec;
        can_msg.header.timestamp_nsec = can_msg_data->timestamp_nsec;
        can_msg.id = can_msg_data->id;
        can_msg.extended = can_msg_data->extended;
        can_msg.rtr = can_msg_data->rtr;
        can_msg.size = static_cast<uint8_t>(PyByteArray_Size(can_msg_data->data));
        memcpy(can_msg.data, PyByteArray_AsString(can_msg_data->data), can_msg.size);

        ret = true;
    }

    return ret;
}

/** \brief Get the object allocation information */
void PyCanMsgType::getPyObjectAllocInfo(newfunc& new_func, size_t& size)
{
    new_func = &PyCanMsgType::pyNew;
    size = sizeof(PyCanMsgObjectData);
}

/** \brief Python interpreter destructor function callback */
void PyCanMsgType::onDealloc(PythonObjectData* self)
{
    PyCanMsgObjectData* can_msg_data = reinterpret_cast<PyCanMsgObjectData*>(self);
    Py_DECREF(can_msg_data->data);
}

/** \brief Python interpreter initialization function callback */
int PyCanMsgType::onInit(PythonObjectData* self, PyObject* args, PyObject* kwds)
{
    int ret = 0;
    static const char* kwlist[] = { "timestamp_sec", "timestamp_nsec", "id", "extended", "rtr", "data", nullptr };
    int64_t timestamp_sec = 0;
    uint32_t timestamp_nsec = 0;
    uint32_t id = 0;
    bool extended = false;
    bool rtr = false;
    PyObject* data = nullptr;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|LkkccY", const_cast<char**>(kwlist), 
                                     &timestamp_sec, &timestamp_nsec, &id, &extended, &rtr, &data))
    {
        return -1;
    }

    PyCanMsgObjectData* can_msg_data = reinterpret_cast<PyCanMsgObjectData*>(self);
    can_msg_data->timestamp_sec = timestamp_sec;
    can_msg_data->timestamp_nsec = timestamp_nsec;
    can_msg_data->id = static_cast<uint32_t>(id);
    can_msg_data->extended = extended;
    can_msg_data->rtr = rtr;
    if (data == nullptr)
    {
        can_msg_data->size = 0;
        can_msg_data->data = PyByteArray_FromStringAndSize("", 0);
    }
    else
    {
        ret = setData(reinterpret_cast<PyObject*>(self), data, nullptr);
    }
    if (can_msg_data->data == nullptr)
    {
        ret = -1;
    }

    return ret;
}

/** \brief Python interpreter constructor function callback */
PyObject* PyCanMsgType::pyNew(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    PyCanMsgType& py_can_msg_type = PyCanMsgType::getInstance();
    PyObject* new_obj = py_can_msg_type.onNew(&py_can_msg_type, type, args, kwds);
    return new_obj;
}

/** \brief Getter for the 'size' member */
PyObject* PyCanMsgType::getSize(PyObject* self, void* closure)
{
    (void*)closure;

    PyCanMsgObjectData* can_msg_data = reinterpret_cast<PyCanMsgObjectData*>(self);
    return PyLong_FromSize_t(can_msg_data->size);
}

/** \brief Setter for the 'size' member */
int PyCanMsgType::setSize(PyObject* self, PyObject* value, void* closure)
{
    (void*)closure;

    // Generic uint setter
    PyCanMsgObjectData* can_msg_data = reinterpret_cast<PyCanMsgObjectData*>(self);
    int ret = PythonType::uintSetter(can_msg_data->size, value, false, 0, 8);
    if (ret == 0)
    {
        // Resize the data field accordingly
        ret = PyByteArray_Resize(can_msg_data->data, static_cast<Py_ssize_t>(can_msg_data->size));
    }
    return ret;
}

/** \brief Getter for the 'data' member */
PyObject* PyCanMsgType::getData(PyObject* self, void* closure)
{
    (void*)closure;

    PyCanMsgObjectData* can_msg_data = reinterpret_cast<PyCanMsgObjectData*>(self);
    Py_INCREF(can_msg_data->data);
    return can_msg_data->data;
}

/** \brief Setter for the 'data' member */
int PyCanMsgType::setData(PyObject* self, PyObject* value, void* closure)
{
    (void*)closure;

    // Generic bytearray setter
    PyCanMsgObjectData* can_msg_data = reinterpret_cast<PyCanMsgObjectData*>(self);
    int ret = PythonType::bytearraySetter(can_msg_data->data, value, false, 8);
    if (ret == 0)
    {
        // Update the size field accordingly
        can_msg_data->size = static_cast<uint8_t>(PyByteArray_Size(can_msg_data->data));
    }

    return ret;
}
