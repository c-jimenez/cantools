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

#ifndef MATHEXPRESSIONTOKEN_H
#define MATHEXPRESSIONTOKEN_H


#include <string>


/** \brief Token of a mathematical expression */
struct MathExpressionToken
{
    /** \brief Types of token */
    enum Type
    {
        CONST_EXPR,
        VAR_EXPR,
        OPERATOR,
        FUNC_CALL
    };

    /** \brief Constructor */
    MathExpressionToken(const std::string& token, const Type type)
    : token(token)
    , type(type)
    {}

    /** \brief String representing the token */
    std::string token;
    /** \brief Type of the token */
    Type type;
};

#endif // MATHEXPRESSIONTOKEN_H
