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
#include "CanAdapterPluginFactory.h"
#include "CanMsgStreamLogger.h"
#include "WhiteListCanMsgFilter.h"
#include "BlackListCanMsgFilter.h"
#include "XmlFileCanMsgDatabase.h"
#include "CanMsgDecoder.h"
#include "DecodedCanMsgStreamLogger.h"

#include <cstdio>

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
                                new Argument<uint32_t>("--white_list", "white_list", "White list => only the selected CAN ids will be displayed", false, 1u, 1000u),
                                new Argument<uint32_t>("--black_list", "black_list", "Black list => the selected CAN ids will never be displayed", false, 1u, 1000u),
                                new Argument<string>("--database", "database", "XML CAN message database file", false)
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
    vector<uint32_t> white_list;
    if (arg_parser["white_list"])
    {
        arg_parser.values<uint32_t>("white_list", white_list);
    }
    vector<uint32_t> black_list;
    if (arg_parser["black_list"])
    {
        arg_parser.values<uint32_t>("black_list", black_list);
    }
    string database = "";
    if (arg_parser["database"])
    {
        database = arg_parser.value<string>("database");
    }

    cout << "-------------------------" << endl;
    cout << "      CAN dump tool" << endl;
    cout << "-------------------------" << endl;
    cout << endl;
    cout << "CAN adapter : " << adapter << endl;
    cout << "CAN adapter parameters : " << adapter_options << endl;
    cout << "CAN bitrate : " << bitrate << "kbit/s" << endl;
    if (arg_parser["white_list"])
    {
        cout << "White list : ";
        cout << "{ " << std::hex;
        for (size_t i = 0; i < white_list.size(); i++)
        {
            cout << "0x" << white_list[i] << " ";
        }
        cout << "}" << std::dec << endl;
    }
    else
    {
        if (arg_parser["black_list"])
        {
            cout << "Black list : ";
            cout << "{ " << std::hex;
            for (size_t i = 0; i < black_list.size(); i++)
            {
                cout << "0x" << black_list[i] << " ";
            }
            cout << "}" << std::dec << endl;
        }
    }
    if (arg_parser["database"])
    {
        cout << "Database : " << database << endl;
    }
    cout << endl;

    cout << "Opening CAN adapter..." << endl;

    // Initialize OS abstraction layer
    if (OSAL_Init())
    {
        // Open database
        XmlFileCanMsgDatabase xml_database;
        if (arg_parser["database"])
        {
            valid = xml_database.load(database);
            if (!valid)
            {
                cout << "Error : unable to load database file => " << xml_database.getLastError() << endl;
            }
        }
        if (valid)
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
                            // Configure filters
                            WhiteListCanMsgFilter white_list_filter;
                            for (size_t i = 0; i < white_list.size(); i++)
                            {
                                white_list_filter.add(white_list[i], false);
                            }

                            BlackListCanMsgFilter black_list_filter;
                            for (size_t i = 0; i < black_list.size(); i++)
                            {
                                black_list_filter.add(black_list[i], false);
                            }

                            CanMsgDecoder can_msg_decoder;
                            can_msg_decoder.setCanMsgDatabase(xml_database);
                            DecodedCanMsgStreamLogger decoded_logger;
                            decoded_logger.setOutputOptions(cout, DecodedCanMsgStreamLogger::OUT_TEXT);
                            can_msg_decoder.setListener(decoded_logger);

                            CanMsgStreamLogger can_logger;
                            can_logger.setOutputOptions(cout, CanMsgStreamLogger::OUT_TEXT, *controller);

                            ICanPipelineStage* listener = nullptr;
                            if (arg_parser["database"])
                            {
                                listener = &can_msg_decoder;
                            }
                            else
                            {
                                listener = &can_logger;
                            }

                            if (white_list.size() > 0)
                            {
                                white_list_filter.addChild(*listener);
                                listener = &white_list_filter;
                            }
                            else
                            {
                                if (black_list.size() > 0)
                                {
                                    black_list_filter.addChild(*listener);
                                    listener = &black_list_filter;
                                }
                            }

                            // Start controller
                            if (controller->start(*listener))
                            {
                                cout << "Waiting for CAN messages, press [Enter] to stop..." << endl;

                                // Wait for enter key
                                getchar();

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
    }
    else
    {
        cerr << "Error : Unable to initialize OS abstraction layer" << endl;
    }

    return ret;
}
