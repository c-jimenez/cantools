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

#ifndef PARAMETERSTRING_H
#define PARAMETERSTRING_H

#include <string>
#include <vector>
#include <map>

/** \brief Represent a parameter string */
class ParameterString
{
    public:


        /** \brief Constructor */
        explicit ParameterString(const char* param_string);
        /** \brief Constructor */
        explicit ParameterString(const std::string& param_string);
        /** \brief Destructor */
        virtual ~ParameterString();


        /** \brief Returns the requested parameter's value as a string */
        const std::string& operator [] (const std::string& parameter_name) const;


        /** \brief Returns the requested parameter's value as a string */
        const std::string& getParam(const std::string& parameter_name, const std::string& default_value = "") const;

        /** \brief Returns the requested parameter's value as a signed integer */
        int64_t getParam(const std::string& parameter_name, const int64_t default_value = 0) const;

        /** \brief Returns the requested parameter's value as an unsigned integer */
        uint64_t getParam(const std::string& parameter_name, const uint64_t default_value = 0) const;

        /** \brief Returns the requested parameter's value as a floating point value */
        double getParam(const std::string& parameter_name, const double default_value = 0.) const;



    private:

        /** \brief Parameters */
        std::map<std::string, std::string> m_parameters;


        /** \brief Initializes the parameter string */
        void init(const std::string& param_string);

        /** \brief Split a string */
        void splitString(const std::string& str, const char split_char, std::vector<std::string>& splitted_str);
};


#endif // PARAMETERSTRING_H
