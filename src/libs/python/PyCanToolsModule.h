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

#ifndef PYCANTOOLSMODULE_H
#define PYCANTOOLSMODULE_H  

#include "PythonModule.h"


/** \brief Module allowing to redirect the Python Execution Environmenent's sys.stdout stream */
class PyCanToolsModule : public PythonModule
{
    public:

        /** \brief Get the unique instance */
        static PyCanToolsModule& getInstance();
        

    protected:

        /** \brief Called to register the module in the inittab */
        virtual bool onRegister() override;


    private:

        /** \brief Constructor */
        PyCanToolsModule();

        /** \brief Destructor */
        virtual ~PyCanToolsModule();


        /** \brief Singleton */
        static PyCanToolsModule* m_singleton;

        /** \brief Method which initialize the module on import */
        static PyObject* pyImportInit();
};

#endif // PYCANTOOLSMODULE_H
