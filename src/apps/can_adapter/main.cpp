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
#include "VectorHelper.h"

#include <iostream>
using namespace std;

/** \brief Application's entry point */
int main(int argc, const char* argv[])
{
    int ret = 1;

    vector<IArgument*> args = {
                                new Argument<string>("--list_adapters", "list_adapters", "List of CAN adapters plugins to use", false, 1u, 100u),
                                new SingleArgument<bool>("--parameters", "parameters", "Display parameters available for each CAN adapter plugin", false),
                                new SingleArgument<bool>("--enumerate", "enumerate", "Enumerate available CAN adapters", false)
                              };
    ArgParser arg_parser(args);
    bool valid = arg_parser.parse(argc, argv);
    if (!valid)
    {
        return ret;
    }

    // Extract parameters
    vector<string> adapters;
    bool list_all = !arg_parser["list_adapters"];
    if (!list_all)
    {
        arg_parser.values<string>("list_adapters", adapters);
    }
    bool parameters = arg_parser["parameters"];
    bool enumerate = arg_parser["enumerate"];

    // Initialize OS abstraction layer
    if (OSAL_Init())
    {
        // Load plugins
        bool res;
        vector<CanAdapterPlugin*> plugins;
        if (list_all)
        {
            res = CanAdapterPluginFactory::load(arg_parser.progPath(), plugins);
        }
        else
        {
            res = CanAdapterPluginFactory::load(arg_parser.progPath(), adapters, plugins);
        }
        if (res)
        {
            // Filter adapter plugins
            cout << plugins.size() << " CAN adapter plugin(s) found";
            if (plugins.size() > 0)
            {
                cout << " : ";
            }
            cout << endl;
            for (size_t i = 0; i < plugins.size(); i++)
            {
                CanAdapterPlugin* adapter_plugin = plugins[i];
                
                // Display plugin information
                cout << endl;
                cout << "- " << adapter_plugin->getName() << " (version " << adapter_plugin->getVersion() << ")";
                if (parameters || enumerate)
                {
                    cout << " : " << endl;
                    if (parameters)
                    {
                        cout << endl;
                        cout << "    - parameters : ";
                        const vector<const PluginParameter*>& params = adapter_plugin->getParameters();
                        if (params.size() == 0)
                        {
                            cout << "none" << endl;
                        }
                        else
                        {
                            cout << endl;
                            for (size_t j = 0; j < params.size(); j++)
                            {
                                const PluginParameter* param = params[j];
                                cout << "        - " << param->name << " (" << param->type << ") : " << param->help << endl;
                            }
                        }
                    }
                    if (enumerate)
                    {
                        cout << endl;
                        cout << "    - available controllers : ";
                        CanControllerEnumerator* enumerator = adapter_plugin->getEnumerator();
                        if (enumerator == nullptr)
                        {
                            cout << "no enumerator available" << endl;
                        }
                        else
                        {
                            vector<CanControllerInformation*> controllers;
                            if (!enumerator->enumerateControllers(controllers))
                            {
                                cout << "error during controller enumeration" << endl;
                            }
                            else
                            {
                                if (controllers.size() == 0)
                                {
                                    cout << "none" << endl;
                                }
                                else
                                {
                                    cout << endl;
                                    for (size_t j = 0; j < controllers.size(); j++)
                                    {
                                        const CanControllerInformation* controller = controllers[j];
                                        cout << "        - " << controller->name << " : " << controller->description << endl;
                                    }
                                }
                            }
                            delete enumerator;
                        }
                    }
                }
                else
                {
                    cout << endl;
                }
            }
        }
        else
        {
            cerr << "Error : Unable to load plugins" << endl;
        }
    }
    else
    {
        cerr << "Error : Unable to initialize OS abstraction layer" << endl;
    }
    
    return ret;
}