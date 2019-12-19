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

#ifndef MATHEXPRESSIONLEXER_H
#define MATHEXPRESSIONLEXER_H

#include "MathExpressionToken.h"

#include <iostream>
#include <vector>
#include <unordered_set>


/** \brief Lexer for mathematical expressions */
class MathExpressionLexer
{
    public:

        /** \brief Constructor */
        MathExpressionLexer();

        /** \brief Destructor */
        virtual ~MathExpressionLexer();


        /** \brief Tokenize a mathematical expression */
        bool tokenize(const std::string& math_expr, std::vector<MathExpressionToken*>& tokens);

        /** \brief Tokenize a mathematical expression */
        bool tokenize(std::istream& math_expr, std::vector<MathExpressionToken*>& tokens);


        /** \brief Get the last error string */
        const std::string& getLastError() { return m_last_error; }

    private:

        /** \brief Last error string */
        std::string m_last_error;


        /** \brief Token's separators */
        static const std::unordered_set<char> m_separators;


        /** \brief Add a token to the token's list */
        bool addToken(const char c, std::string& token, std::vector<MathExpressionToken*>& tokens);

        /** \brief Check if a token represents a numeric value */
        bool isNumeric(const std::string token, bool& is_numeric);
};


#endif // MATHEXPRESSIONLEXER_H
