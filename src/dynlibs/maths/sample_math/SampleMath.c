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

#include "MathPluginApi.h"
#include "SampleFunctions.h"


/** \brief Get the plugin's type */
static const char* DYN_LINK_API MATH_PLUGIN_GetType(void)
{
    return MATH_PLUGIN_TYPE;
}

/** \brief Get the plugin's name */
static const char* DYN_LINK_API MATH_PLUGIN_GetName(void)
{
    return "sampl_math";
}

/** \brief Get the plugin's version */
static const char* DYN_LINK_API MATH_PLUGIN_GetVersion(void)
{
    return "1.0";
}

/** \brief Get the plugin's parameters */
static const PluginParameter* DYN_LINK_API MATH_PLUGIN_GetParams(void)
{
    static const PluginParameter plugin_params[] = 
    {
        {NULL, NULL, NULL}
    };

    return plugin_params;
}


/** \brief Returns the value of PI */
double SAMPLE_FUNCTION_PiValue(void);

/** \brief Convert an angle value from deg to rad */
double SAMPLE_FUNCTION_ToRad(double deg_value);

/** \brief Convert an angle value from rad to deg */
double SAMPLE_FUNCTION_ToDeg(double rad_value);

/** \brief Convert a speed value from m/s to km/h */
double SAMPLE_FUNCTION_MsToKmh(double ms_value);

/** \brief Computes the hypothenuse of a triangle */
double SAMPLE_FUNCTION_Hypothenuse(double a, double b);

/** \brief Get the math functions */
static const MathFunctionDesc* DYN_LINK_API MATH_PLUGIN_GetFunctions(void)
{
    static const MathFunctionDesc math_functions[] =
    {
        {"pi_val", &SAMPLE_FUNCTION_PiValue, 0},
        {"torad", &SAMPLE_FUNCTION_ToRad, 1u},
        {"todeg", &SAMPLE_FUNCTION_ToDeg, 1u},
        {"ms_to_kmh", &SAMPLE_FUNCTION_MsToKmh, 1u},
        {"hypo", &SAMPLE_FUNCTION_Hypothenuse, 2u},
        {NULL, NULL, 0}
    };

    return math_functions;
}


/** \brief Math's plugin functions */
static const MathPluginFuncs math_plugin_funcs = {
                                                    &MATH_PLUGIN_GetFunctions
                                                 };

/** \brief Plugin's generic functions */
static const PluginGenericFuncs plugin_generic_funcs = {
                                                        &MATH_PLUGIN_GetType,
                                                        &MATH_PLUGIN_GetName,
                                                        &MATH_PLUGIN_GetVersion,
                                                        &MATH_PLUGIN_GetParams,
                                                        &math_plugin_funcs
                                                       };


/** \brief Get the plugin's functions */
DYN_LINK_EXPORT const PluginGenericFuncs* DYN_LINK_API PLUGIN_GetFuncs()
{
    return &plugin_generic_funcs;
}
