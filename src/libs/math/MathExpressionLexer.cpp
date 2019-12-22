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

#include "MathExpressionLexer.h"

#include <sstream>

using namespace std;


/** \brief Token's separators */
const std::unordered_set<char> MathExpressionLexer::m_separators = { '(', ')', '+', '-', '*', '/', ',' };


/** \brief Constructor */
MathExpressionLexer::MathExpressionLexer()
: m_last_error("")
{}

/** \brief Destructor */
MathExpressionLexer::~MathExpressionLexer()
{}

/** \brief Tokenize a mathematical expression */
bool MathExpressionLexer::tokenize(const std::string& math_expr, std::vector<MathExpressionToken*>& tokens)
{
    stringstream math_expr_stream;
    math_expr_stream << math_expr;
    return tokenize(math_expr_stream, tokens);
}

/** \brief Tokenize a mathematical expression */
bool MathExpressionLexer::tokenize(std::istream& math_expr, std::vector<MathExpressionToken*>& tokens)
{
    bool ret = true;
    string token = "";
    m_last_error = "";

    // Clear existing list
    tokens.clear();
    while (ret && !math_expr.eof())
    {
        // Extract char
        char c = 0;
        math_expr >> c;
        if (!math_expr.eof())
        {
            // Look for separator
            auto iter = m_separators.find(c);
            if (iter != m_separators.end())
            {
                // Add token to the list
                ret = addToken(c, token, tokens);
            }
            else
            {
                // Construct the token's string
                token.append(&c, 1u);
            }
        }
    }

    // Add final token
    ret = addToken(0, token, tokens);

    return ret;
}

/** \brief Add a token to the token's list */
bool MathExpressionLexer::addToken(const char c, std::string& token, std::vector<MathExpressionToken*>& tokens)
{
    bool ret = true;

    if (!token.empty())
    {
        // Constants and variables
        bool is_numeric = false;
        MathExpressionToken* tok = nullptr;
        ret = isNumeric(token, is_numeric);
        if (ret)
        {
            if (is_numeric)
            {
                tok = new MathExpressionToken(token, MathExpressionToken::CONST_EXPR);
            }
            else
            {
                tok = new MathExpressionToken(token, MathExpressionToken::VAR_EXPR);
            }
            tokens.push_back(tok);
            token = "";
        }
    }
    if (c != 0)
    {
        MathExpressionToken* op_token = nullptr;
        if (c == '(')
        {
            // Function call or computation block
            if (tokens.size() != 0)
            {
                MathExpressionToken* prev_token = tokens[tokens.size() - 1];
                if (prev_token->type == MathExpressionToken::VAR_EXPR)
                {
                    op_token = new MathExpressionToken(prev_token->token, MathExpressionToken::FUNC_CALL);
                    tokens[tokens.size() - 1] = op_token;
                    delete prev_token;
                }
            }
        }
        if (op_token == nullptr)
        {
            op_token = new MathExpressionToken(std::string(&c, 1u), MathExpressionToken::OPERATOR);
            tokens.push_back(op_token);
        }
    }

    return ret;
}

/** \brief Check if a token represents a numeric value */
bool MathExpressionLexer::isNumeric(const std::string token, bool& is_numeric)
{
    bool ret = true;
    bool dot_found = false;

    // Check if the token contains only numeric chars i=with at most 1 '.'
    is_numeric = true;
    for (size_t i = 0; (is_numeric && (i < token.size())); i++)
    {
        if (token[i] == '.')
        {
            if (!dot_found)
            {
                dot_found = true;
            }
            else
            {
                ret = false;

                stringstream error;
                error << "Invalid number representation : " << token;
                m_last_error = error.str();
            }
        }
        else
        {
            is_numeric = ((token[i] >= '0') && (token[i] <= '9'));
        }
    }

    // Check if the variable name doesn't begin with numerical value
    if (!is_numeric &&
        (token[0] >= '0') && 
        (token[0] <= '9'))
    {
        ret = false;

        stringstream error;
        error << "Invalid variable name : " << token;
        m_last_error = error.str();
    }

    return ret;
}

