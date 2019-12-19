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

#define PY_SSIZE_T_CLEAN
#include "Python.h"


#include "PythonExecutionEnvironment.h"
#include "PyCanToolsModule.h"
#include "PyStdOstreamModule.h"
#include "PyCanMsgType.h"

#include <iostream>
using namespace std;

int main(int argc, const char* argv[])
{
    {
        std::cout << "Hello World!\n";

        PythonExecutionEnvironment& pyee = PythonExecutionEnvironment::getInstance();

        PyCanToolsModule& pyee_cantools = PyCanToolsModule::getInstance();
        pyee.addModule(pyee_cantools);

        PyCanMsgType& pyee_can_msg = PyCanMsgType::getInstance();
        pyee_cantools.addType(pyee_can_msg);


        PyStdOstreamModule pyee_stdout("pyee_stdout", "sys.stdout redirecton", PyStdOstreamModule::OS_STDOUT);
        //    pyee.addModule(pyee_stdout);
        PyStdOstreamModule pyee_stderr("pyee_stderr", "sys.stderr redirecton", PyStdOstreamModule::OS_STDERR);
        //  pyee.addModule(pyee_stderr);

        if (pyee.initialize(argv[0]))
        {

            PyRun_SimpleString("import sys\nfrom time import time,ctime\nimport cantools\n"
                "print('Today is', ctime(time()))\n"
                "sys.stderr.write('pouf!')\n"
                "can_msg = cantools.CanMsg()\n"
                "print(str(can_msg.data))\n"
                "can_msg.data = bytearray(8)\n"
                "print(str(can_msg.data))\n"
                "print(str(can_msg.size))\n"
                "can_msg.size = 1\n"
                "print(str(can_msg.data))\n"
                "print(str(can_msg.size))\n"
                "can_msg.data[0] = 12\n"
                "print(str(can_msg.data))\n"
                "can_msg.size = 8\n"
                "print(str(can_msg.data))\n"
                "print(str(can_msg.size))\n");

            char data_s[5u] = { 0, 1, 2, 3, 4 };
            PyObject* data = PyByteArray_FromStringAndSize(data_s, 5u);
            PyObject* params = Py_BuildValue("LkkccO", -1234567890123456789, 987654u, 123u, true, false, data);
            PyObject* can_msg = pyee_can_msg.create(params);
            Py_DECREF(params);
            Py_DECREF(data);

            CanMsg msg = { 0 };
            pyee_can_msg.toCanMsg(can_msg, msg);
            Py_DECREF(can_msg);

            PyObject* can_msg2 = pyee_can_msg.fromCanMsg(msg);
            CanMsg msg2 = { 0 };
            pyee_can_msg.toCanMsg(can_msg2, msg2);

            Py_DECREF(can_msg2);

            pyee.release();
        }
        PythonExecutionEnvironment::deleteInstance();
    }
    return 0;
}
