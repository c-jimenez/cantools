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
#include "CanPipeline.h"
#include "CanMsgForwarder.h"
#include "WhiteListCanMsgFilter.h"
#include "BlackListCanMsgFilter.h"

#include <cstdio>

#include <iostream>
#include <thread>
#include <chrono>
using namespace std;



/** \brief Initialize a CAN adapter */
static CanController* InitAdapter(const string& plugin_path, const string& adapter, const string& adapter_options, const uint16_t bitrate);


/** \brief Application's entry point */
int main(int argc, const char* argv[])
{
    int ret = 1;

    vector<IArgument*> args = {
                                new Argument<string>("--adapter1", "adapter1", "CAN adapter 1 name", true),
                                new Argument<string>("--options1", "option_string1", "Option string for CAN adapter 1", false),
                                new Argument<string>("--adapter2", "adapter2", "CAN adapter 2 name", true),
                                new Argument<string>("--options2", "option_string2", "Option string for CAN adapter 2", false),
                                new Argument<uint16_t>("--bitrate", "bitrate", "CAN bitrate in kbit/s", true),
                                new Argument<uint32_t>("--white_list1", "white_list1", "White list for CAN adapter 1 => only the selected CAN ids will be transitted to the CAN adapter", false, 1u, 1000u),
                                new Argument<uint32_t>("--black_list1", "black_list1", "Black list for CAN adapter 1 => the selected CAN ids will never be transitted to the CAN adapter", false, 1u, 1000u),
                                new Argument<uint32_t>("--white_list2", "white_list2", "White list for CAN adapter 2 => only the selected CAN ids will be transitted to the CAN adapter", false, 1u, 1000u),
                                new Argument<uint32_t>("--black_list2", "black_list2", "Black list for CAN adapter 2 => the selected CAN ids will never be transitted to the CAN adapter", false, 1u, 1000u)
                              };
    ArgParser arg_parser(args);
    bool valid = arg_parser.parse(argc, argv);
    if (!valid)
    {
        return ret;
    }

    // Extract parameters
    string adapter1 = arg_parser.value<string>("adapter1");
    string adapter_options1 = "";
    if (arg_parser["option_string1"])
    {
        adapter_options1 = arg_parser.value<string>("option_string1");
    }
    string adapter2 = arg_parser.value<string>("adapter2");
    string adapter_options2 = "";
    if (arg_parser["option_string2"])
    {
        adapter_options2 = arg_parser.value<string>("option_string2");
    }
    uint16_t bitrate = arg_parser.value<uint16_t>("bitrate");
    vector<uint32_t> white_list1;
    if (arg_parser["white_list1"])
    {
        arg_parser.values<uint32_t>("white_list1", white_list1);
    }
    vector<uint32_t> black_list1;
    if (arg_parser["black_list1"])
    {
        arg_parser.values<uint32_t>("black_list1", black_list1);
    }
    vector<uint32_t> white_list2;
    if (arg_parser["white_list2"])
    {
        arg_parser.values<uint32_t>("white_list2", white_list2);
    }
    vector<uint32_t> black_list2;
    if (arg_parser["black_list2"])
    {
        arg_parser.values<uint32_t>("black_list2", black_list2);
    }

    cout << "-------------------------" << endl;
    cout << "      CAN bridge tool" << endl;
    cout << "-------------------------" << endl;
    cout << endl;
    cout << "CAN adapter 1 : " << adapter1 << endl;
    cout << "CAN adapter 1 parameters : " << adapter_options1 << endl;
    cout << "CAN adapter 2 : " << adapter2 << endl;
    cout << "CAN adapter 2 parameters : " << adapter_options2 << endl;
    cout << "CAN bitrate : " << bitrate << "kbit/s" << endl;
    cout << endl;
    if (arg_parser["white_list1"])
    {
        cout << "White list for CAN adapter 1 : ";
        cout << "{ " << std::hex;
        for (size_t i = 0; i < white_list1.size(); i++)
        {
            cout << "0x" << white_list1[i] << " ";
        }
        cout << "}" << std::dec << endl;
    }
    else
    {
        if (arg_parser["black_list1"])
        {
            cout << "Black list for CAN adapter 1 : ";
            cout << "{ " << std::hex;
            for (size_t i = 0; i < black_list1.size(); i++)
            {
                cout << "0x" << black_list1[i] << " ";
            }
            cout << "}" << std::dec << endl;
        }
    }
    if (arg_parser["white_list2"])
    {
        cout << "White list for CAN adapter 2 : ";
        cout << "{ " << std::hex;
        for (size_t i = 0; i < white_list2.size(); i++)
        {
            cout << "0x" << white_list2[i] << " ";
        }
        cout << "}" << std::dec << endl;
    }
    else
    {
        if (arg_parser["black_list2"])
        {
            cout << "Black list for CAN adapter 2 : ";
            cout << "{ " << std::hex;
            for (size_t i = 0; i < black_list2.size(); i++)
            {
                cout << "0x" << black_list2[i] << " ";
            }
            cout << "}" << std::dec << endl;
        }
    }

    // Initialize OS abstraction layer
    if (OSAL_Init())
    {
        // Create pipeline objects
        CanPipeline pipeline1("pipeline_1");
        CanPipeline pipeline2("pipeline_2");
        WhiteListCanMsgFilter white_list_filter1;
        WhiteListCanMsgFilter white_list_filter2;
        BlackListCanMsgFilter black_list_filter1;
        BlackListCanMsgFilter black_list_filter2;
        CanMsgForwarder forwarder1;
        CanMsgForwarder forwarder2;

        // Build pipelines
        if (white_list2.size() > 0)
        {
            pipeline1.addChild(white_list_filter2);
            white_list_filter2.addChild(forwarder1);
            for (size_t i = 0; i < white_list2.size(); i++)
            {
                white_list_filter2.add(white_list2[i], false);
            }
        }
        else if (black_list2.size() > 0)
        {
            pipeline1.addChild(black_list_filter2);
            black_list_filter2.addChild(forwarder1);
            for (size_t i = 0; i < black_list2.size(); i++)
            {
                black_list_filter2.add(black_list2[i], false);
            }
        }
        else
        {
            pipeline1.addChild(forwarder1);
        }
        if (white_list1.size() > 0)
        {
            pipeline2.addChild(white_list_filter1);
            white_list_filter1.addChild(forwarder2);
            for (size_t i = 0; i < white_list1.size(); i++)
            {
                white_list_filter1.add(white_list1[i], false);
            }
        }
        else if (black_list1.size() > 0)
        {
            pipeline2.addChild(black_list_filter1);
            black_list_filter1.addChild(forwarder2);
            for (size_t i = 0; i < black_list1.size(); i++)
            {
                black_list_filter1.add(black_list1[i], false);
            }
        }
        else
        {
            pipeline2.addChild(forwarder2);
        }

        // Initialize adapters
        cout << "Opening CAN adapter 1..." << endl;
        CanController* controller1 = InitAdapter(arg_parser.progPath(), adapter1, adapter_options1, bitrate);
        if (controller1 != nullptr)
        {
            cout << "Opening CAN adapter 2..." << endl;
            CanController* controller2 = InitAdapter(arg_parser.progPath(), adapter2, adapter_options2, bitrate);
            if (controller2 != nullptr)
            {
                // Configure forwarders
                forwarder1.setTargetController(*controller2);
                forwarder2.setTargetController(*controller1);
                
                // Start pipelines
                bool ret = pipeline1.start(*controller1);
                if (ret)
                {
                    ret = pipeline2.start(*controller2);
                    if (ret)
                    {
                        cout << "Bridge started, press [Enter] to stop..." << endl;

                        // Wait for enter key
                        getchar();

                        // Stop pipelines
                        pipeline1.stop();
                        pipeline2.stop();
                    }
                    else
                    {
                        cout << "Error : unable to start CAN controller 2 => " << pipeline2.getErrorMsg() << "endl";
                    }
                }
                else
                {
                    cout << "Error : unable to start CAN controller 1 => " << pipeline1.getErrorMsg() << "endl";
                }
                delete controller2;
            }
            else
            {
                delete controller1;
            }
        }
    }
    else
    {
        cerr << "Error : Unable to initialize OS abstraction layer" << endl;
    }

    return ret;
}

/** \brief Initialize a CAN adapter */
static CanController* InitAdapter(const string& plugin_path, const string& adapter, const string& adapter_options, const uint16_t bitrate)
{
    CanController* controller = nullptr;

    // Load plugin
    CanAdapterPlugin* can_adapter_plugin = CanAdapterPluginFactory::load(plugin_path, adapter);
    if (can_adapter_plugin != nullptr)
    {
        // Instanciate controller
        controller = can_adapter_plugin->createController(adapter_options.c_str());
        if (controller != nullptr)
        {
            // Open controller
            if (controller->open())
            {
                // Configure controller
                if (!controller->configure(static_cast<CanBitrate>(bitrate), CANMODE_STD | CANMODE_EXT))
                {
                    cerr << "Error : Unable to configure CAN controller => " << controller->getErrorMsg() << endl;
                    controller->close();
                    delete controller;
                    controller = nullptr;
                    delete can_adapter_plugin;
                }
            }
            else
            {
                cerr << "Error : Unable to open CAN controller => " << controller->getErrorMsg() << endl;
                delete controller;
                controller = nullptr;
                delete can_adapter_plugin;
            }
        }
        else
        {
            cerr << "Error : Unable to instanciate CAN controller with specified options => " << adapter_options << endl;
            delete can_adapter_plugin;
        }
    }
    else
    {
        cerr << "Error : CAN adapter plugin [" << adapter << "] not found" << endl;
    }

    return controller;
}