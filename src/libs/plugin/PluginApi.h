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

#ifndef PLUGIN_API_H
#define PLUGIN_API_H

#include "PluginHelper.h"

/** \brief Plugin parameter's description */
typedef struct _PluginParameter
{
    /** \brief Name */
    const char* name;
    /** \brief Type */
    const char* type;
    /** \brief Help string */
    const char* help;
} PluginParameter;


/** \brief Get the plugin's type */
typedef const char* (DYN_LINK_API *fp_plugin_get_type_t)(void);

/** \brief Get the plugin's name */
typedef const char* (DYN_LINK_API *fp_plugin_get_name_t)(void);

/** \brief Get the plugin's version */
typedef const char* (DYN_LINK_API *fp_plugin_get_version_t)(void);

/** \brief Get the plugin's parameters */
typedef const PluginParameter* (DYN_LINK_API *fp_plugin_get_params_t)(void);


/** \brief Plugin's generic functions */
typedef struct _PluginGenericFuncs
{
    fp_plugin_get_type_t get_type;
    fp_plugin_get_name_t get_name;
    fp_plugin_get_version_t get_version;
    fp_plugin_get_params_t get_params;
    const void* plugin_specific_funcs;
} PluginGenericFuncs;

/** \brief Get the plugins's functions */
typedef const PluginGenericFuncs* (DYN_LINK_API *fp_plugin_get_funcs_t)(void);

/** \brief Name of the plugin function */
#define FP_PLUGIN_GET_FUNCS_NAME   "PLUGIN_GetFuncs"


#endif // PLUGIN_API_H
