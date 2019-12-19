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

#include "MathPlugin.h"
#include "MathExpression.h"

using namespace std;

/** \brief Constructor */
MathPlugin::MathPlugin(DynLibLoader* loader, const PluginGenericFuncs* plugin_funcs)
: Plugin(loader, plugin_funcs)
, m_plugin_funcs(reinterpret_cast<const MathPluginFuncs*>(plugin_funcs->plugin_specific_funcs))
{}

/** \brief Destructor */
MathPlugin::~MathPlugin()
{}

/** \brief Get the math functions */
void MathPlugin::getMathFunctions(std::vector<std::pair<std::string, FunctionCallExpression*>>& functions)
{
    const MathFunctionDesc* plugin_function = m_plugin_funcs->get_functions();
    while (plugin_function->function != nullptr)
    {
        if ((plugin_function->name != nullptr) &&
            (plugin_function->param_count <= 4))
        {
            string func_name = plugin_function->name;
            FunctionCallExpression* func_expr = nullptr;

            if (plugin_function->param_count == 0)
            {
                typedef double(*fp_func_t)(void);
                fp_func_t func = nullptr;
                reinterpret_cast<void*&>(func) = plugin_function->function;
                func_expr = new FunctionCall0Expression(FUNC_CALL_EXPR_BIND0(*func));
            }
            else if (plugin_function->param_count == 1u)
            {
                typedef double(*fp_func_t)(double);
                fp_func_t func = nullptr;
                reinterpret_cast<void*&>(func) = plugin_function->function;
                func_expr = new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(*func));
            }
            else if (plugin_function->param_count == 2u)
            {
                typedef double(*fp_func_t)(double, double);
                fp_func_t func = nullptr;
                reinterpret_cast<void*&>(func) = plugin_function->function;
                func_expr = new FunctionCall2Expression(FUNC_CALL_EXPR_BIND2(*func));
            }
            else if (plugin_function->param_count == 3u)
            {
                typedef double(*fp_func_t)(double, double, double);
                fp_func_t func = nullptr;
                reinterpret_cast<void*&>(func) = plugin_function->function;
                func_expr = new FunctionCall3Expression(FUNC_CALL_EXPR_BIND3(*func));
            }
            else
            {
                typedef double(*fp_func_t)(double, double, double, double);
                fp_func_t func = nullptr;
                reinterpret_cast<void*&>(func) = plugin_function->function;
                func_expr = new FunctionCall4Expression(FUNC_CALL_EXPR_BIND4(*func));
            }
            functions.push_back(pair<string, FunctionCallExpression*>(func_name, func_expr));
        }
        plugin_function++;
    }
}
