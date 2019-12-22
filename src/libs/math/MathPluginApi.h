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

#ifndef MATHPLUGIN_API_H
#define MATHPLUGIN_API_H

#include "PluginApi.h"


/** \brief Math plugin type */
#define MATH_PLUGIN_TYPE "math"


/** \brief Math function description */
typedef struct _MathFunctionDesc
{
    /** \brief Function name */
    const char* name;
    /** \brief Function address */
    void* function;
    /** \brief Number of parameters */
    uint8_t param_count;
} MathFunctionDesc;



/** \brief Get the math functions */
typedef const MathFunctionDesc* (DYN_LINK_API *fp_math_get_functions_t)(void);


/** \brief Math's plugin functions */
typedef struct _MathPluginFuncs
{
    fp_math_get_functions_t get_functions;
} MathPluginFuncs;


#endif // MATHPLUGIN_API_H
