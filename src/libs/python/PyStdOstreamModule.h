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

#ifndef PYSTDOSTREAMMODULE_H
#define PYSTDOSTREAMMODULE_H  

#include "PythonModule.h"

#include <sstream>


/** \brief Module allowing to redirect the Python Execution Environmenent's sys.stdout or sys.stderr streams */
class PyStdOstreamModule : public PythonModule
{
    public:

        /** \brief Listener class */
        class Listener
        {
            public:

                /** \brief Called when data is available on the stream */
                virtual void onData(const std::string& data) = 0;
        };

        /** \brief Output streams */
        enum OutputStream
        {
            /** \brief sys.stdout */
            OS_STDOUT,
            /** \brief sys.stderr */
            OS_STDERR
        };


        /** \brief Constructor */
        PyStdOstreamModule(const std::string& name, const std::string& doc, const OutputStream out_stream);

        /** \brief Destructor */
        virtual ~PyStdOstreamModule();


    protected:

        /** \brief Called during initialization */
        virtual bool onInit() override;


    private:

        /** \brief Output stream */
        const OutputStream m_out_stream;

        /** \brief Data buffer */
        std::stringstream m_data;

        /** \brief Listener */
        Listener* m_listener;


        /** \brief Method which bufferize data written to the stream */
        static PyObject* write(PyObject* self, PyObject* args);

        /** \brief Method which notify data written to the stream to the listener */
        static PyObject* flush(PyObject* self, PyObject* args);
};

#endif // PYSTDOSTREAMMODULE_H
