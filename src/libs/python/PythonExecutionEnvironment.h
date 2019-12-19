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

#ifndef PYTHONEXECUTIONENVIRONMENT_H
#define PYTHONEXECUTIONENVIRONMENT_H


#include <string>
#include <map>

class PythonModule;

/** \brief Singleton which initialize a Python Execution Environmnent within the application (not thread safe) */
class PythonExecutionEnvironment
{
    public:

        /** \brief Get the unique instance */
        static PythonExecutionEnvironment& getInstance();

        /** \brief Delete the unique instance */
        static void deleteInstance();


        /** \brief Add a module to the execution environment */
        bool addModule(PythonModule& module);


        /** \brief Initialize the execution environment */
        bool initialize(const std::string& prog_path);

        /** \brief Release the execution environment */
        void release();


        /** \brief Get the last error string */
        const std::string& getLastError() const { return m_last_error; }


    private:

        /** \brief Last error string */
        std::string m_last_error;

        /** \brief Decoded program name */
        wchar_t* m_program;

        /** \brief List of modules */
        std::map<std::string, PythonModule*> m_modules;



        /** \brief Constructor */
        PythonExecutionEnvironment();

        /** \brief Destructor */
        ~PythonExecutionEnvironment();

        /** \brief Singleton */
        static PythonExecutionEnvironment* m_singleton;

        /** \brief Module initialisation */
        static void* moduleInit();
};

#endif // PYTHONEXECUTIONENVIRONMENT_H
