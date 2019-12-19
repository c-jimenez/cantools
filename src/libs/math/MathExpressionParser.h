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

#ifndef MATHEXPRESSIONPARSER_H
#define MATHEXPRESSIONPARSER_H

#include "MathExpression.h"
#include "MathExpressionLexer.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <deque>
#include <unordered_set>
#include <memory>

/** \brief Parser for mathematical expressions */
class MathExpressionParser
{
    public:

        /** \brief Constructor */
        MathExpressionParser();

        /** \brief Destructor */
        virtual ~MathExpressionParser();


        /** \brief Add user defined mathematical functions */
        void addUserFunctions(std::vector<std::pair<std::string, FunctionCallExpression*>>& functions);


        /** \brief Parse a mathematical expression */
        bool parse(const std::string& math_expr_str, std::unique_ptr<IMathExpression>& math_expr);

        /** \brief Parse a mathematical expression */
        bool parse(std::istream& math_expr_str, std::unique_ptr<IMathExpression>& math_expr);



        /** \brief Get the last error string */
        const std::string& getLastError() { return m_last_error; }

    private:

        /** \brief Last error string */
        std::string m_last_error;

        /** \brief Lexer */
        MathExpressionLexer m_lexer;

        /** \brief User defined mathematical functions */
        std::map<std::string, FunctionCallExpression*> m_user_functions;


        /** \brief Built-in mathematical functions */
        static const std::map<std::string, FunctionCallExpression*> m_built_in_functions;

        /** \brief Prioritary operators in a mathematical expression */
        static const std::unordered_set<std::string> m_prioritary_operators;



        /** \brief Check the syntax of the mathematical exression */
        bool checkSyntax(const std::vector<MathExpressionToken*>& tokens);

        /** \brief Recursively parse a mathematical expression */
        bool parseExpression(const std::vector<MathExpressionToken*> tokens, size_t& index, bool need_ending_parenthesis, IMathExpression*& math_expr);

        /** \brief Factorize expression to obtain a unique mathematical expression object which holds the while mathematical expression object tree */
        bool factorizeExpression(std::deque<MathExpressionToken*>& exprs_tokens, std::deque<IMathExpression*>exprs, IMathExpression*& math_expr);

        /** \brief Parse the parameters of a function call in a mathematical expression */
        bool parseFunctionParams(const std::string& function, const std::vector<MathExpressionToken*>& tokens, size_t& index, std::vector<IMathExpression*>& params);

        /** \brief Look for a function in built-in and user functions */
        bool findFunction(const std::string& function_name, FunctionCallExpression*& func_expr);

};


#endif // MATHEXPRESSIONPARSER_H
