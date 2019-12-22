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

#ifndef PYCANMSGTYPE_H
#define PYCANMSGTYPE_H  

#include "PythonType.h"
#include "CanData.h"

#include <cstdint>

/** \brief Class representing a python CAN message type */
class PyCanMsgType : public PythonType
{
    public:

        /** \brief Get the unique instance */
        static PyCanMsgType& getInstance();
        
        /** \brief Create a python CAN message */
        PyObject* fromCanMsg(const CanMsg& can_msg);

        /** \brief Convert as a CAN message */
        bool toCanMsg(const PyObject* py_can_msg, CanMsg& can_msg);


    protected:

        /** \brief Python object data */
        struct PyCanMsgObjectData : public PythonObjectData
        {
            /** \brief Timestamp in seconds since 1970/01/01 - 00:00:00 in UTC */
            int64_t timestamp_sec;
            /** \brief Integral number of nanoseconds to add to the timestamp in seconds */
            uint32_t timestamp_nsec;

            /** \brief Identifier */
            uint32_t id;
            /** \brief Indicate if it is an extended identifier */
            bool extended;
            /** \brief Indicate if it is a remote request */
            bool rtr;
            /** \brief Data size */
            size_t size;
            /** \brief Data as a bytearray */
            PyObject* data;
        };


        /** \brief Get the object allocation information */
        virtual void getPyObjectAllocInfo(newfunc& new_func, size_t& size) override;

        /** \brief Python interpreter destructor function callback */
        virtual void onDealloc(PythonObjectData* self) override;

        /** \brief Python interpreter initialization function callback */
        virtual int onInit(PythonObjectData* self, PyObject* args, PyObject* kwds) override;


    private:

        /** \brief Constructor */
        PyCanMsgType();

        /** \brief Destructor */
        virtual ~PyCanMsgType();


        /** \brief Singleton */
        static PyCanMsgType* m_singleton;


        /** \brief Python interpreter constructor function callback */
        static PyObject* pyNew(PyTypeObject* type, PyObject* args, PyObject* kwds);


        /** \brief Getter for the 'size' member */
        static PyObject* getSize(PyObject* self, void* closure);
        /** \brief Setter for the 'size' member */
        static int setSize(PyObject* self, PyObject* value, void* closure);

        /** \brief Getter for the 'data' member */
        static PyObject* getData(PyObject* self, void* closure);
        /** \brief Setter for the 'data' member */
        static int setData(PyObject* self, PyObject* value, void* closure);
};


#endif // PYCANMSGTYPE_H
