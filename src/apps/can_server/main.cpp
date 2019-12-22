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

#include "osal.h"
#include "ArgParser.h"
#include "CanServer.h"

#include <cstdio>

#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

/** \brief Application's entry point */
int main(int argc, const char* argv[])
{
    int ret = 1;

    vector<IArgument*> args = {
                                new Argument<string>("--ip", "ip", "Listening address. Default = all available IP addresses", false),
                                new Argument<uint16_t>("--port", "port", "Listening port. Default = 6789", false),
                                new Argument<uint32_t>("--timeout", "port", "Listening port. Default = 500", false)
                              };
    ArgParser arg_parser(args);
    bool valid = arg_parser.parse(argc, argv);
    if (!valid)
    {
        return ret;
    }

    // Extract parameters
    string ip_address = "";
    if (arg_parser["ip"])
    {
        ip_address = arg_parser.value<string>("ip");
    }
    uint16_t port = 6789u;
    if (arg_parser["port"])
    {
        port = arg_parser.value<uint16_t>("port");
    }
    uint32_t timeout = 500u;
    if (arg_parser["timeout"])
    {
        port = arg_parser.value<uint32_t>("timeout");
    }

    cout << "-------------------------" << endl;
    cout << "     CAN server tool" << endl;
    cout << "-------------------------" << endl;
    cout << endl;
    cout << "Listening IP address : " << (ip_address.empty() ? "all" : ip_address) << endl;
    cout << "Listening port : " << port << endl;
    cout << "Timeout : " << timeout << "ms" << endl;
    cout << endl;

    cout << "Starting server..." << endl;

    // Initialize OS abstraction layer
    if (OSAL_Init())
    {
        CanServer can_server;
        if (can_server.start(ip_address, port, timeout))
        {
            cout << "Server started, press [Enter] to stop..." << endl;

            // Wait for enter key
            getchar();
        }
        else
        {
            cerr << "Error : Unable to start server" << endl;
        }
    }
    else
    {
        cerr << "Error : Unable to initialize OS abstraction layer" << endl;
    }

    return ret;
}
