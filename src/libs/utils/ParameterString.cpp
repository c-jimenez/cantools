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

#include "ParameterString.h"

#include <sstream>

using namespace std;

/** \brief Constructor */
ParameterString::ParameterString(const char* param_string)
: m_parameters()
{
    if (param_string != nullptr)
    {
        init(param_string);
    }
}

/** \brief Constructor */
ParameterString::ParameterString(const std::string& param_string)
: m_parameters()
{
    init(param_string);
}

/** \brief Destructor */
ParameterString::~ParameterString()
{}

/** \brief Returns the requested parameter's value */
const std::string& ParameterString::operator [] (const std::string& parameter_name) const
{
    static const std::string empty_str = "";

    auto iter = m_parameters.find(parameter_name);
    if (iter != m_parameters.end())
    {
        return iter->second;
    }
    else
    {
        return empty_str;
    }
}

/** \brief Returns the requested parameter's value as a string */
const std::string& ParameterString::getParam(const std::string& parameter_name, const std::string& default_value) const
{
    const string& value_str = this->operator[](parameter_name);
    if (value_str.empty())
    {
        return default_value;
    }
    return value_str;
}

/** \brief Returns the requested parameter's value as a signed integer */
int64_t ParameterString::getParam(const std::string& parameter_name, const int64_t default_value) const
{
    int64_t value = default_value;
    string value_str = this->operator[](parameter_name);
    if (!value_str.empty())
    {
        value = static_cast<int64_t>(std::stoll(value_str));
    }
    return value;
}

/** \brief Returns the requested parameter's value as an unsigned integer */
uint64_t ParameterString::getParam(const std::string& parameter_name, const uint64_t default_value) const
{
    uint64_t value = default_value;
    string value_str = this->operator[](parameter_name);
    if (!value_str.empty())
    {
        value = static_cast<uint64_t>(std::stoull(value_str));
    }
    return value;
}

/** \brief Returns the requested parameter's value as a floating point value */
double ParameterString::getParam(const std::string& parameter_name, const double default_value) const
{
    double value = default_value;
    string value_str = this->operator[](parameter_name);
    if (!value_str.empty())
    {
        value = std::stold(value_str);
    }
    return value;
}


/** \brief Initializes the parameter string */
void ParameterString::init(const std::string& param_string)
{
    vector<string> parameters;
    vector<string> param_value;
    splitString(param_string, ';', parameters);
    for (size_t i = 0; i < parameters.size(); i++)
    {
        splitString(parameters[i], '=', param_value);
        if ((param_value.size() == 2u) &&
            !param_value[0u].empty() &&
            !param_value[1u].empty())
        {
            m_parameters[param_value[0u]] = param_value[1u];
        }
    }
}

/** \brief Split a string */
void ParameterString::splitString(const std::string& str, const char split_char, std::vector<std::string>& splitted_str)
{
    splitted_str.clear();

    size_t pos;
    size_t offset = 0;
    while ((pos = str.find(split_char, offset)) != string::npos)
    {
        splitted_str.push_back(str.substr(offset, pos - offset));
        offset = pos + 1;
    }
    splitted_str.push_back(str.substr(offset));
}
