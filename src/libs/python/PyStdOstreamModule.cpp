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

#include "PyStdOstreamModule.h"


/** \brief Constructor */
PyStdOstreamModule::PyStdOstreamModule(const std::string& name, const std::string& doc, const OutputStream out_stream)
: PythonModule(name, doc, false)
, m_out_stream(out_stream)
, m_data()
, m_listener(nullptr)
{
    addMethod(PyMethodDef{ "write", &PyStdOstreamModule::write, METH_VARARGS, "Bufferize data written to the stream" });
    addMethod(PyMethodDef{ "flush", &PyStdOstreamModule::flush, METH_VARARGS, "Notify data written to the stream" });
}

/** \brief Destructor */
PyStdOstreamModule::~PyStdOstreamModule()
{}

/** \brief Called during initialization */
bool PyStdOstreamModule::onInit()
{
    bool ret;

    if (m_out_stream == OS_STDOUT)
    {
        ret = (PySys_SetObject("stdout", getPyObject()) == 0);
    }
    else
    {
        ret = (PySys_SetObject("stderr", getPyObject()) == 0);
    }

    return ret;
}

/** \brief Method which bufferize data written to the stream */
PyObject* PyStdOstreamModule::write(PyObject* self, PyObject* args)
{
    PyStdOstreamModule* py_ostream = PythonModule::getModule<PyStdOstreamModule>(self);
    const char *what;
    if (!PyArg_ParseTuple(args, "s", &what))
    {
        return nullptr;
    }
    py_ostream->m_data << what;
    return Py_BuildValue("");
}

/** \brief Method which notify data written to the stream to the listener */
PyObject* PyStdOstreamModule::flush(PyObject* self, PyObject* args)
{
    PyStdOstreamModule* py_ostream = PythonModule::getModule<PyStdOstreamModule>(self);
    (void*)args;
    if (py_ostream->m_listener != nullptr)
    {
        py_ostream->m_listener->onData(py_ostream->m_data.str());
    }
    py_ostream->m_data.clear();
    return Py_BuildValue("");
}
