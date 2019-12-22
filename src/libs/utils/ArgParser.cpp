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

#include "ArgParser.h"
#include "VectorHelper.h"

#include <iostream>
using namespace std;


/** \brief Constructor */
ArgParser::ArgParser(std::vector<IArgument*>& args, bool auto_print_help, bool add_help_argument)
: m_args(args)
, m_values()
, m_args_presence()
, m_prog_name("")
, m_prog_path("")
, m_auto_print_help(auto_print_help)
, m_help_argument(nullptr)
{
    if (add_help_argument)
    {
        m_help_argument = new SingleArgument<bool>("-h", "help", "Display this help message", false, true);
        args.push_back(m_help_argument);
    }
}

/** \brief Destructor */
ArgParser::~ArgParser()
{
    // Free memory allocated for the converters
    releaseConverters();
}


/** \brief Parse the command line arguments */
bool ArgParser::parse(int argc, const char* argv[])
{
    bool ret = false;
    bool full_help = false;

    if (argc > 0)
    {
        // Extract program name
        size_t offset;
        m_prog_name = argv[0];
        m_prog_path = "";
        offset = m_prog_name.rfind('\\');
        if (offset == string::npos)
        {
            offset = m_prog_name.rfind('/');
        }
        if (offset != string::npos)
        {
            m_prog_path = m_prog_name.substr(0, offset + 1u);
            m_prog_name = m_prog_name.substr(offset + 1u);
        }

        // Initialize internal data structures
        resetPresenceState();
        releaseConverters();
        ret = true;

        // Parse command line arguments
        int count = 1;
        vector<IArgumentValueConverter*> converters;
        while (ret && (count != argc))
        {
            // Look for the argument in the list
            std::string option_name = argv[count];
            IArgument* arg = findArgument(option_name);
            if (arg == nullptr)
            {
                cerr << "Unknown option : " << option_name << endl;
                ret = false;
            }
            else if (arg == m_help_argument)
            {
                // Displya full help message
                full_help = true;
                ret = false;
            }
            else
            {
                // Update argument presence state
                m_args_presence[arg] = true;

                // Check if the argument as parameters
                converters.clear();
                if (arg->maxcount() == 0)
                {
                    // Single option
                    converters.push_back(arg->converter());
                    count++;
                }
                else
                {
                    // Parse parameter
                    count++;
                    bool stop = false;
                    while ((count != argc) && ret && !stop)
                    {
                        // Check if parameter is an option
                        std::string parameter = argv[count];
                        if (findArgument(parameter) != nullptr)
                        {
                            stop = true;
                        }
                        else
                        {
                            // Convert parameter value
                            IArgumentValueConverter* converter = arg->converter();
                            ret = converter->convert(parameter);
                            if (ret)
                            {
                                converters.push_back(converter);
                                count++;
                            }
                            else
                            {
                                cerr << "Invalid parameter value for option [" << arg->option() << "] => " << parameter << endl;
                            }
                        }
                    }
                    if (ret)
                    {
                        // Check number of parameters
                        if ((converters.size() < arg->mincount()) ||
                            (converters.size() > arg->maxcount()))
                        {
                            cerr << "Invalid number of parameters for option [" << arg->option() << "] => " << converters.size() << endl;
                            ret = false;
                        }
                    }
                }
                if (ret)
                {
                    // Save values
                    m_values[arg->name()] = converters;
                }
            }
        }
        if (ret)
        {
            // Check mandatory arguments presence
            for (size_t i = 0; i < m_args.size(); i++)
            {
                IArgument* arg = m_args[i];
                if (arg->mandatory())
                {
                    if (!m_args_presence[arg])
                    {
                        cerr << "Missing mandatory option : " << arg->option() << endl;
                        ret = false;
                    }
                }
            }
        }
    }

    // Automatic help display
    if (!ret && m_auto_print_help)
    {
        printHelp(full_help);
    }

    return ret;
}

/** \brief Print the help message */
void ArgParser::printHelp(const bool full)
{
    // Minimal usage
    cout << "Usage : " << m_prog_name << " ";
    for (size_t i = 0; i < m_args.size(); i++)
    {
        IArgument* arg = m_args[i];
        if (!arg->mandatory())
        {
            cout << "[";
        }
        cout << arg->option();
        if (arg->mincount() > 0)
        {
            cout << " " << arg->name();
            if ((arg->maxcount() != arg->mincount()) ||
                (arg->mincount() > 1))
            {
                cout << "...";
            }
        }
        if (!arg->mandatory())
        {
            cout << "]";
        }
        cout << " ";
    }
    cout << endl;

    // Detailed usage
    if (full)
    {
        cout << endl;
        cout << "Options :" << endl;
        for (size_t i = 0; i < m_args.size(); i++)
        {
            IArgument* arg = m_args[i];
            cout << "\t" << arg->option();
            if (arg->mincount() > 0)
            {
                cout << " " << arg->name();
                if (arg->maxcount() != arg->mincount())
                {
                    cout << "...[" << arg->mincount() << "-" << arg->maxcount() << "]";
                }
                else
                {
                    if (arg->mincount() > 1)
                    {
                        cout << "...[" << arg->mincount() << "]";
                        cout << "\t";
                    }
                    else
                    {
                        cout << "\t\t";
                    }
                }
            }
            else
            {
                cout << "\t\t";
            }
            cout << "\t" << arg->helper() << endl;
        }
        cout << endl;
    }
}




/** \brief Reset the presence state of arguments */
void ArgParser::resetPresenceState()
{
    for (size_t i = 0; i < m_args.size(); i++)
    {
        m_args_presence[m_args[i]] = false;
    }
}

/** \brief Release the memory allocated for the converters */
void ArgParser::releaseConverters()
{
    for (auto iter = m_values.begin(); iter != m_values.end(); ++iter)
    {
        vector<IArgumentValueConverter*>& converters = iter->second;
        delete_all(converters);
    }
    m_values.clear();
}

/** \brief Look for an argument by its option's name */
IArgument* ArgParser::findArgument(const std::string& option_name)
{
    IArgument* arg = nullptr;
    for (size_t i = 0; (i < m_args.size()) && (arg == nullptr); i++)
    {
        if (m_args[i]->option() == option_name)
        {
            arg = m_args[i];
        }
    }
    return arg;
}
