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

#ifndef ARGCHECKER_H
#define ARGCHECKER_H

#include <string>
#include <locale>
#include <limits>
#include <vector>
#include <map>

#include "StringHelper.h"
#include "NoCopy.h"


/** \brief Interface for all the argument value converters */
class IArgumentValueConverter
{
    public:

        /** \brief Destructor */
        virtual ~IArgumentValueConverter() {}


        /** \brief Convert a value */
        virtual bool convert(const std::string& value) = 0;

        /** \brief Get the converted value */
        virtual const void* value() const = 0;
};

/** \brief Generic argument value conversion function */
template <typename T>
static inline bool generic_argument_value_converter(const std::string& value, T& res)
{
    (void)value;
    (void)res;
    return false;
}

/** \brief Generic argument value conversion function => specialization for std::string type */
template <>
inline bool generic_argument_value_converter<std::string>(const std::string& value, std::string& res)
{
    res = value;
    return true;
}

/** \brief Generic argument value conversion function => specialization for bool type */
template <>
inline bool generic_argument_value_converter<bool>(const std::string& value, bool& res)
{
    bool ret = true;
    std::string lower_value = tolower(value);
    if (lower_value.compare("true") == 0)
    {
        res = true;
    }
    else if (lower_value.compare("false") == 0)
    {
        res = false;
    }
    else
    {
        ret = false;
    }
    return ret;
}

/** \brief Argument value conversion function for unsigned integer type */
template <typename UnsignedIntegerType>
static inline bool generic_argument_value_converter_uint(const std::string& value, UnsignedIntegerType& res)
{
    bool ret = true;
    uint64_t temp = 0;
    try
    {
        // Check hexadecimal value
        if ((value.size() > 2) &&
            (value.substr(0u, 2u) == "0x"))
        {
            temp = std::stoull(value.substr(2u, value.size() - 2u), 0, 16);
        }
        else
        {
            temp = std::stoull(value);
        }
    }
    catch(...)
    {
        ret = false;
    }
    if (ret)
    {
        ret = (temp <= static_cast<uint64_t>(std::numeric_limits<UnsignedIntegerType>::max()));
        if (ret)
        {
            res = static_cast<UnsignedIntegerType>(temp);
        }
    }
    return ret;
}

/** \brief Generic argument value conversion function => specialization for 8 bits unsigned integer type */
template <>
inline bool generic_argument_value_converter<uint8_t>(const std::string& value, uint8_t& res)
{
    return generic_argument_value_converter_uint<uint8_t>(value, res);
}

/** \brief Generic argument value conversion function => specialization for 16 bits unsigned integer type */
template <>
inline bool generic_argument_value_converter<uint16_t>(const std::string& value, uint16_t& res)
{
    return generic_argument_value_converter_uint<uint16_t>(value, res);
}

/** \brief Generic argument value conversion function => specialization for 32 bits unsigned integer type */
template <>
inline bool generic_argument_value_converter<uint32_t>(const std::string& value, uint32_t& res)
{
    return generic_argument_value_converter_uint<uint32_t>(value, res);
}

/** \brief Generic argument value conversion function => specialization for 64 bits unsigned integer type */
template <>
inline bool generic_argument_value_converter<uint64_t>(const std::string& value, uint64_t& res)
{
    return generic_argument_value_converter_uint<uint64_t>(value, res);
}

/** \brief Argument value conversion function for signed integer type */
template <typename SignedIntegerType>
static inline bool generic_argument_value_converter_sint(const std::string& value, SignedIntegerType& res)
{
    bool ret = true;
    int64_t temp = 0;
    try
    {
        temp = std::stoll(value);
    }
    catch (...)
    {
        ret = false;
    }
    if (ret)
    {
        ret = ((temp >= static_cast<int64_t>(std::numeric_limits<SignedIntegerType>::min())) &&
               (temp <= static_cast<int64_t>(std::numeric_limits<SignedIntegerType>::max())));
        if (ret)
        {
            res = static_cast<SignedIntegerType>(temp);
        }
    }
    return ret;
}

/** \brief Generic argument value conversion function => specialization for 8 bits signed integer type */
template <>
inline bool generic_argument_value_converter<int8_t>(const std::string& value, int8_t& res)
{
    return generic_argument_value_converter_sint<int8_t>(value, res);
}

/** \brief Generic argument value conversion function => specialization for 16 bits signed integer type */
template <>
inline bool generic_argument_value_converter<int16_t>(const std::string& value, int16_t& res)
{
    return generic_argument_value_converter_sint<int16_t>(value, res);
}

/** \brief Generic argument value conversion function => specialization for 32 bits signed integer type */
template <>
inline bool generic_argument_value_converter<int32_t>(const std::string& value, int32_t& res)
{
    return generic_argument_value_converter_sint<int32_t>(value, res);
}

/** \brief Generic argument value conversion function => specialization for 64 bits signed integer type */
template <>
inline bool generic_argument_value_converter<int64_t>(const std::string& value, int64_t& res)
{
    return generic_argument_value_converter_sint<int64_t>(value, res);
}


/** \brief Argument value conversion function for floating point type */
template <typename FloatType>
static inline bool generic_argument_value_converter_float(const std::string& value, FloatType& res)
{
    bool ret = true;
    double temp = 0;
    try
    {
        temp = std::stod(value);
    }
    catch (...)
    {
        ret = false;
    }
    if (ret)
    {
        ret = ((temp >= static_cast<double>(std::numeric_limits<FloatType>::min())) &&
            (temp <= static_cast<double>(std::numeric_limits<FloatType>::max())));
        if (ret)
        {
            res = static_cast<FloatType>(temp);
        }
    }
    return ret;
}

/** \brief Generic argument value conversion function => specialization for single precision floating point type */
template <>
inline bool generic_argument_value_converter<float>(const std::string& value, float& res)
{
    return generic_argument_value_converter_float<float>(value, res);
}

/** \brief Generic argument value conversion function => specialization for double precision floating point type */
template <>
inline bool generic_argument_value_converter<double>(const std::string& value, double& res)
{
    return generic_argument_value_converter_float<double>(value, res);
}



/** \brief Generic class for all the argument value converters */
template <typename T>
class GenericArgumentValueConverter: public IArgumentValueConverter
{
    public:

        /** \brief Constructor */
        GenericArgumentValueConverter()
        : m_value()
        {}

        /** \brief Constructor */
        GenericArgumentValueConverter(const T& value)
        : m_value(value)
        {}

        /** \brief Destructor */
        virtual ~GenericArgumentValueConverter()
        {}


        /** \brief Convert a value */
        virtual bool convert(const std::string& value) override
        {
            return generic_argument_value_converter<T>(value, m_value);
        }

        /** \brief Get the converted value */
        virtual const void* value() const override { return &m_value; }


    private:

        /** \brief Internal value */
        T m_value;
};


/** \brief Interface for all argument types */
class IArgument
{
    public:

        /** \brief Destructor */
        virtual ~IArgument() {}


        /** \brief Option's name */
        virtual const std::string& option() const = 0;

        /** \brief Name */
        virtual const std::string& name() const = 0;

        /** \brief Helper string */
        virtual const std::string& helper() const = 0;

        /** \brief Indicate if the argument is mandatory */
        virtual bool mandatory() const = 0;

        /** \brief Indicate the minimum number of values which must follow the argument */
        virtual size_t mincount() const = 0;

        /** \brief Indicate the maximum number of values which must follow the argument */
        virtual size_t maxcount() const = 0;

        /** \brief Create a new argument value converter */
        virtual IArgumentValueConverter* converter() const = 0;
};

/** \brief Generic class for all arguments implementations */
template <typename T>
class Argument : public IArgument, public INoCopy
{
    public:

        /** \brief Constructor */
        Argument(const std::string& option, const std::string& name, const std::string& helper, const bool mandatory, const size_t min_count, const size_t max_count)
        : m_option(option)
        , m_name(name)
        , m_helper(helper)
        , m_mandatory(mandatory)
        , m_min_count(min_count)
        , m_max_count(max_count)
        {}

        /** \brief Constructor */
        Argument(const std::string& option, const std::string& name, const std::string& helper, const bool mandatory, const size_t count)
        : m_option(option)
        , m_name(name)
        , m_helper(helper)
        , m_mandatory(mandatory)
        , m_min_count(count)
        , m_max_count(count)
        {}

        /** \brief Constructor */
        Argument(const std::string& option, const std::string& name, const std::string& helper, const bool mandatory)
        : m_option(option)
        , m_name(name)
        , m_helper(helper)
        , m_mandatory(mandatory)
        , m_min_count(1u)
        , m_max_count(1u)
        {}

        
        /** \brief Destructor */
        virtual ~Argument() 
        {}


        /** \brief Option's name */
        virtual const std::string& option() const override { return m_option; }

        /** \brief Name */
        virtual const std::string& name() const override { return m_name; }

        /** \brief Helper string */
        virtual const std::string& helper() const override { return m_helper; }

        /** \brief Indicate if the argument is mandatory */
        virtual bool mandatory() const override { return m_mandatory; }

        /** \brief Indicate the minimum number of values which must follow the argument */
        virtual size_t mincount() const override { return m_min_count; }

        /** \brief Indicate the maximum number of values which must follow the argument */
        virtual size_t maxcount() const override { return m_max_count; }

        /** \brief Create a new argument value converter */
        virtual IArgumentValueConverter* converter() const override { return new GenericArgumentValueConverter<T>(); }


    private:

        /** \brief Option's name */
        const std::string m_option;

        /** \brief Name */
        const std::string m_name;

        /** \brief Helper string */
        const std::string m_helper;

        /** \brief Indicate if the argument is mandatory */
        bool m_mandatory;

        /** \brief Indicate the minimum number of values which must follow the argument */
        size_t m_min_count;

        /** \brief Indicate the maximum number of values which must follow the argument */
        size_t m_max_count;
};

/** \brief Generic class for arguments with no parameters implementations */
template <typename T>
class SingleArgument : public IArgument, public INoCopy
{
    public:

        /** \brief Constructor */
        SingleArgument(const std::string& option, const std::string& name, const std::string& helper, const bool mandatory, const T& value)
        : m_option(option)
        , m_name(name)
        , m_helper(helper)
        , m_mandatory(mandatory)
        , m_value(value)
        {}

        /** \brief Constructor */
        SingleArgument(const std::string& option, const std::string& name, const std::string& helper, const bool mandatory)
        : m_option(option)
        , m_name(name)
        , m_helper(helper)
        , m_mandatory(mandatory)
        , m_value()
        {}


        /** \brief Destructor */
        virtual ~SingleArgument()
        {}


        /** \brief Option's name */
        virtual const std::string& option() const override { return m_option; }

        /** \brief Name */
        virtual const std::string& name() const override { return m_name; }

        /** \brief Helper string */
        virtual const std::string& helper() const override { return m_helper; }

        /** \brief Indicate if the argument is mandatory */
        virtual bool mandatory() const override { return m_mandatory; }

        /** \brief Indicate the minimum number of values which must follow the argument */
        virtual size_t mincount() const override { return 0; }

        /** \brief Indicate the maximum number of values which must follow the argument */
        virtual size_t maxcount() const override { return 0; }

        /** \brief Create a new argument value converter */
        virtual IArgumentValueConverter* converter() const override { return new GenericArgumentValueConverter<T>(m_value); }


    private:

        /** \brief Option's name */
        const std::string m_option;

        /** \brief Name */
        const std::string m_name;

        /** \brief Helper string */
        const std::string m_helper;

        /** \brief Indicate if the argument is mandatory */
        bool m_mandatory;

        /** \brief Indicate the minimum number of values which must follow the argument */
        size_t m_min_count;

        /** \brief Indicate the maximum number of values which must follow the argument */
        size_t m_max_count;

        /** \brief Replacement value */
        T m_value;
};


/** \brief Utility class to parse command line arguments */
class ArgParser : public INoCopy
{
    public:

        /** \brief Constructor */
        ArgParser(std::vector<IArgument*>& args, bool auto_print_help = true, bool add_help_argument = true);
        /** \brief Destructor */
        virtual ~ArgParser();


        /** \brief Parse the command line arguments */
        bool parse(int argc, const char* argv[]);

        /** \brief Print the help message */
        void printHelp(const bool full);


        /** \brief Get the program's name */
        const std::string& progName() const { return m_prog_name;  }

        /** \brief Get the program's path */
        const std::string& progPath() const { return m_prog_path; }

        
        /** \brief Indicate if an argument is present in the command line */
        bool operator [] (const std::string& name)
        {
            return (m_values.find(name) != m_values.end());
        }

        /** \brief Get the number of values of an argument */
        size_t count(const std::string& name)
        {
            return m_values[name].size();
        }

        /** \brief Get the value of an argument */
        template <class T>
        const T& value(const std::string& name, size_t instance = 0)
        {
            return *reinterpret_cast<const T*>(m_values[name][instance]->value());
        }

        /** \brief Get the values of an argument */
        template <class T>
        void values(const std::string& name, std::vector<T>& vals)
        {
            vals.clear();

            const std::vector<IArgumentValueConverter*> converters = m_values[name];
            for (size_t i = 0; i < converters.size(); i++)
            {
                vals.push_back(*reinterpret_cast<const T*>(converters[i]->value()));
            }
        }


    private:

        /** \brief Arguments */
        const std::vector<IArgument*>& m_args;

        /** \brief Values */
        std::map<std::string, std::vector<IArgumentValueConverter*>> m_values;

        /** \brief Presence state of arguments */
        std::map<IArgument*, bool> m_args_presence;

        /** \brief Program name */
        std::string m_prog_name;

        /** \brief Program path */
        std::string m_prog_path;

        /** \brief Indicate if the help must be displayed automatically in case of error or for the -h option */
        bool m_auto_print_help;

        /** \brief Automatic help argument */
        IArgument* m_help_argument;



        /** \brief Reset the presence state of arguments */
        void resetPresenceState();

        /** \brief Release the memory allocated for the converters */
        void releaseConverters();

        /** \brief Look for an argument by its option's name */
        IArgument* findArgument(const std::string& option_name);

};

#endif // ARGCHECKER_H
