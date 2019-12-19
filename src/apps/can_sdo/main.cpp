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
#include "PluginLoader.h"
#include "CanAdapterPluginFactory.h"
#include "CanSdoReader.h"
#include "CanSdoWriter.h"

#include <iostream>
using namespace std;

/** \brief Application's entry point */
int main(int argc, const char* argv[])
{
    int ret = 1;

    vector<IArgument*> args = {
                                new Argument<string>("--adapter", "adapter", "CAN adapter name", true),
                                new Argument<string>("--options", "option_string", "Option string for the selected CAN adapter", false),
                                new Argument<uint16_t>("--bitrate", "bitrate", "CAN bitrate in kbit/s", true),
                                new Argument<uint16_t>("--client_sdo", "client_sdo_id", "CAN id for the client SDO message", true),
                                new Argument<uint16_t>("--server_sdo", "server_sdo_id", "CAN id for the client SDO message", true),
                                new Argument<uint16_t>("--obd_index", "obd_index", "CANOpen OBD index", true),
                                new Argument<uint8_t>("--obd_subindex", "obd_subindex", "CANOpen OBD subindex", true),
                                new Argument<string>("--data_string", "data_string", "Data to write using SDO represented as a string", false),
                                new Argument<uint8_t>("--data_bytes", "data_bytes", "Data to write using SDO represented as a bytes", false, 1u, 4096u),
                                new SingleArgument<bool>("--read", "read_mode", "Indicate an SDO read operation", false),
                                new SingleArgument<bool>("--string_output", "string_output", "Output read data as a string instead of a byte array", false)
                              };
    ArgParser arg_parser(args);
    bool valid = arg_parser.parse(argc, argv);
    if (!valid)
    {
        return ret;
    }

    // Extract parameters
    string adapter = arg_parser.value<string>("adapter");
    string adapter_options = ""; 
    if (arg_parser["option_string"])
    {
        adapter_options = arg_parser.value<string>("option_string");
    }
    uint16_t bitrate = arg_parser.value<uint16_t>("bitrate");
    uint16_t client_sdo = arg_parser.value<uint16_t>("client_sdo_id");
    uint16_t server_sdo = arg_parser.value<uint16_t>("server_sdo_id");
    bool read = arg_parser["read_mode"];
    bool string_output = arg_parser["string_output"];
    uint16_t obd_index = arg_parser.value<uint16_t>("obd_index");
    uint8_t obd_subindex = arg_parser.value<uint8_t>("obd_subindex");
    string data_string = "";
    if (arg_parser["data_string"])
    {
        data_string = arg_parser.value<string>("data_string");
    }
    vector<uint8_t> data_bytes;
    if (arg_parser["data_bytes"])
    {
        arg_parser.values<uint8_t>("data_bytes", data_bytes);
    }
    else
    {
        for (size_t i = 0; i < data_string.size(); i++)
        {
            data_bytes.push_back(static_cast<uint8_t>(data_string[i]));
        }
    }

    cout << "-------------------------" << endl;
    cout << "      CAN SDO tool" << endl;
    cout << "-------------------------" << endl;
    cout << endl;
    cout << "CAN adapter : " << adapter << endl;
    cout << "CAN adapter parameters : " << adapter_options << endl;
    cout << "CAN bitrate : " << bitrate << "kbit/s" << endl;
    cout << endl;
    cout << "CANopen client SDO : 0x" << std::hex << client_sdo << endl;
    cout << "CANopen server SDO : 0x" << std::hex << server_sdo << endl;
    cout << "CANopen OBD index : 0x" << std::hex << obd_index << endl;
    cout << "CANopen OBD subindex : 0x" << std::hex << (int)obd_subindex << endl;
    cout << endl;
    if (!read)
    {
        if (arg_parser["data_bytes"])
        {
            cout << "Data to write (" << std::dec << data_bytes.size() << " byte(s)) : ";
            cout << "{ " << std::hex;
            for (size_t i = 0; i < data_bytes.size(); i++)
            {
                cout << "0x" << (int)data_bytes[i] << " ";
            }
            cout << "}" << std::dec << endl;
        }
        else
        {
            cout << "Data to write (" << std::dec << data_string.size() << " byte(s)) : " << data_string << endl;
        }
        cout << endl;
    }

    cout << "Opening CAN adapter..." << endl;

    // Initialize OS abstraction layer
    if (OSAL_Init())
    {
        // Load plugin
        CanAdapterPlugin* can_adapter_plugin = CanAdapterPluginFactory::load(arg_parser.progPath(), adapter);
        if (can_adapter_plugin != nullptr)
        {
            // Instanciate controller
            CanController* controller = can_adapter_plugin->createController(adapter_options.c_str());
            if (controller != nullptr)
            {
                // Open controller
                if (controller->open())
                {
                    // Configure controller
                    if (controller->configure(static_cast<CanBitrate>(bitrate), CANMODE_STD | CANMODE_EXT))
                    {
                        // Start controller
                        CanSdoReader sdo_reader(*controller);
                        CanSdoWriter sdo_writer(*controller);
                        ICanControllerListener* listener = nullptr;
                        if (read)
                        {
                            listener = &sdo_reader;
                        }
                        else
                        {
                            listener = &sdo_writer;
                        }
                        if (controller->start(*listener))
                        {
                            if (read)
                            {
                                // SDO read
                                cout << "Starting read..." << endl;
                                if (sdo_reader.read(client_sdo, server_sdo, obd_index, obd_subindex))
                                {
                                    cout << std::dec << (int)sdo_reader.getSize() << " byte(s) received : "  << endl;
                                    if (string_output)
                                    {
                                        cout << reinterpret_cast<const char*>(sdo_reader.getData()) << endl;
                                    }
                                    else
                                    {
                                        const uint8_t* data = sdo_reader.getData();
                                        cout << "{ " << std::hex;
                                        for (uint32_t i = 0; i < sdo_reader.getSize(); i++)
                                        {
                                            cout << "0x" << (int)data[i] << " ";
                                        }
                                        cout << "}" << endl;
                                    }
                                }
                                else
                                {
                                    cerr << "Error : SDO read operation failed => " << sdo_reader.getErrorMsg() << endl;
                                }
                            }
                            else
                            {
                                // SDO write
                                cout << "Starting write..." << endl;
                                if (sdo_writer.write(client_sdo, server_sdo, obd_index, obd_subindex, data_bytes.data(), static_cast<uint32_t>(data_bytes.size())))
                                {
                                    cout << (int)data_bytes.size() << " byte(s) sent!";
                                }
                                else
                                {
                                    cerr << "Error : SDO write operation failed => " << sdo_writer.getErrorMsg() << endl;
                                }
                            }

                            // Stop controller
                            controller->stop();
                        }
                        else
                        {
                            cerr << "Error : Unable to start CAN controller => " << controller->getErrorMsg() << endl;
                        }
                    }
                    else
                    {
                        cerr << "Error : Unable to configure CAN controller => " << controller->getErrorMsg() << endl;
                    }

                    // Close controller
                    controller->close();
                }
                else
                {
                    cerr << "Error : Unable to open CAN controller => " << controller->getErrorMsg() << endl;
                }

                // Release controller
                delete controller;
            }
            else
            {
                cerr << "Error : Unable to instanciate CAN controller with specified options => " << adapter_options << endl;
            }
        }
        else
        {
            cerr << "Error : CAN adapter plugin [" << adapter << "] not found" << endl;
        }
    }
    else
    {
        cerr << "Error : Unable to initialize OS abstraction layer" << endl;
    }

    return ret;
}
