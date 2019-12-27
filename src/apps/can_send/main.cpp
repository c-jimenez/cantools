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
#include "ICanControllerListener.h"

#include <cstdio>
#include <cstring>

#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

/** \brief Dummy CAN listener which does nothing at all */
class DummyCanControllerListener : public ICanControllerListener
{
    public:

        /** \brief Called when CAN data has been received on the controller */
        virtual void canDataReceived(const CanData& can_data) override { (void)can_data; }
};


/** \brief Application's entry point */
int main(int argc, const char* argv[])
{
    int ret = 1;

    vector<IArgument*> args = {
                                new Argument<string>("--adapter", "adapter", "CAN adapter name", true),
                                new Argument<string>("--options", "option_string", "Option string for the selected CAN adapter", false),
                                new Argument<uint16_t>("--bitrate", "bitrate", "CAN bitrate in kbit/s", true),
                                new Argument<uint32_t>("--msg_id", "msg_id", "CAN message identifier", true),
                                new Argument<uint8_t>("--msg_data", "msg_data", "CAN message data", false, 1u, 8u),
                                new SingleArgument<bool>("--extended", "extended", "Indicate that this is a message with an extended identifier", false),
                                new SingleArgument<bool>("--rtr", "rtr", "Indicate that this is a remote transmission request message", false)
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
    uint32_t msg_id = arg_parser.value<uint16_t>("msg_id");
    vector<uint8_t> msg_data;
    if (arg_parser["msg_data"])
    {
        arg_parser.values<uint8_t>("msg_data", msg_data);
    }
    bool extended = arg_parser["extended"];
    bool rtr = arg_parser["rtr"];

    cout << "-------------------------" << endl;
    cout << "      CAN send tool" << endl;
    cout << "-------------------------" << endl;
    cout << endl;
    cout << "CAN adapter : " << adapter << endl;
    cout << "CAN adapter parameters : " << adapter_options << endl;
    cout << "CAN bitrate : " << bitrate << "kbit/s" << endl;
    cout << endl;
    cout << "Message : ";
    cout << "id = 0x" << std::hex << msg_id << std::dec << " - ";
    cout << "flags = ";
    if (extended)
    {
        cout << "ext ";
    }
    if (rtr)
    {
        cout << "rtr ";
    }
    if (!extended && !rtr)
    {
        cout << "none ";
    }
    cout << "- ";
    cout << "size = " << msg_data.size() << " - ";
    cout << "data = { " << std::hex;
    for (size_t i = 0; i < msg_data.size(); i++)
    {
        cout << "0x" << (int)msg_data[i] << " ";
    }
    cout << "}" << std::dec << endl;


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
                        DummyCanControllerListener dummy_listener;
                        if (controller->start(dummy_listener))
                        {
                            cout << "Sending message..." << endl;

                            CanMsg can_msg = { 0 };
                            can_msg.id = msg_id;
                            can_msg.extended = extended;
                            can_msg.rtr = rtr;
                            can_msg.size = static_cast<uint8_t>(msg_data.size());
                            memcpy(can_msg.data, &msg_data[0], can_msg.size);
                            if (controller->send(can_msg))
                            {
                                // Wait for message to be effectly sent
                                this_thread::sleep_for(chrono::milliseconds(100u));

                                cout << "Done!" << endl;
                            }
                            else
                            {
                                cerr << "Error : Unable to send CAN message => " << controller->getErrorMsg() << endl;
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
