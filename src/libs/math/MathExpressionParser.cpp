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

#include "MathExpressionParser.h"
#include "DequeHelper.h"
#include "MapHelper.h"

#include <cmath>
#include <sstream>
using namespace std;



/** \brief Built-in mathematical functions */
const std::map<std::string, FunctionCallExpression*> MathExpressionParser::m_built_in_functions = 
{
    { "cos", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::cos)) },
    { "sin", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::sin)) },
    { "tan", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::tan)) },
    { "acos", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::acos)) },
    { "asin", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::asin)) },
    { "atan", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::atan)) },

    { "exp", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::exp)) },
    { "log", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::log)) },
    { "log10", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::log10)) },

    { "sqrt", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::sqrt)) },
    { "cbrt", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::cbrt)) },

    { "ceil", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::ceil)) },
    { "floor", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::floor)) },
    { "round", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::round)) },
    { "trunc", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::trunc)) },

    { "abs", new FunctionCall1Expression(FUNC_CALL_EXPR_BIND1(std::abs)) },

    { "min", new FunctionCall2Expression(FUNC_CALL_EXPR_BIND2(std::fmin)) },
    { "max", new FunctionCall2Expression(FUNC_CALL_EXPR_BIND2(std::fmax)) },
    { "pow", new FunctionCall2Expression(FUNC_CALL_EXPR_BIND2(std::pow)) }
};

/** \brief Prioritary operators in a mathematical expression */
const std::unordered_set<std::string> MathExpressionParser::m_prioritary_operators = { "*", "/" };





/** \brief Constructor */
MathExpressionParser::MathExpressionParser()
: m_last_error("")
, m_lexer()
, m_user_functions()
{}

/** \brief Destructor */
MathExpressionParser::~MathExpressionParser()
{
    delete_all(m_user_functions);
}

/** \brief Add user defined mathematical functions */
void MathExpressionParser::addUserFunctions(std::vector<std::pair<std::string, FunctionCallExpression*>>& functions)
{
    for (size_t i = 0; i < functions.size(); i++)
    {
        m_user_functions[functions[i].first] = functions[i].second->clone();
    }
}

/** \brief Parse a mathematical expression */
bool MathExpressionParser::parse(const std::string& math_expr_str, std::unique_ptr<IMathExpression>& math_expr)
{
    stringstream math_expr_stream;
    math_expr_stream << math_expr_str;
    return parse(math_expr_stream, math_expr);
}

/** \brief Parse a mathematical expression */
bool MathExpressionParser::parse(std::istream& math_expr_str, std::unique_ptr<IMathExpression>& math_expr)
{
    bool ret = true;
    m_last_error = "";
    math_expr = nullptr;

    // Tokenize expression
    vector<MathExpressionToken*> tokens;
    ret = m_lexer.tokenize(math_expr_str, tokens);
    if (ret)
    {
        // Check syntax
        ret = checkSyntax(tokens);
        if (ret)
        {
            // Parse expression recursively
            size_t index = 0;
            IMathExpression* expr = nullptr;
            ret = parseExpression(tokens, index, false, expr);
            if (ret)
            {
                math_expr = std::unique_ptr<IMathExpression>(expr);
            }
        }
    }
    else
    {
        m_last_error = m_lexer.getLastError();
    }

    // Release memory
    delete_all(tokens);

    return ret;
}

/** \brief Check the syntax of the mathematical exression */
bool MathExpressionParser::checkSyntax(const std::vector<MathExpressionToken*>& tokens)
{
    bool ret = true;
    int parentheses_count = 0;
    for (size_t i = 0; ret && (i < tokens.size()); i++)
    {
        const MathExpressionToken* token = tokens[i];
        switch (token->type)
        {
            case MathExpressionToken::CONST_EXPR:
                /* Intended fallthrough */
            case MathExpressionToken::VAR_EXPR:
                /* Intended fallthrough */
            case MathExpressionToken::FUNC_CALL:
            {
                if (i != 0)
                {
                    // Previous token must be an operator
                    ret = (tokens[i - 1]->type == MathExpressionToken::OPERATOR);
                    if (!ret)
                    {
                        stringstream error;
                        error << "Expression [" << token->token << "] must be preceded by an operator";
                        m_last_error = error.str();
                    }
                }
                if (i != (tokens.size() - 1))
                {
                    // Next token must be an operator, except for function call
                    ret = ((tokens[i + 1]->type == MathExpressionToken::OPERATOR) ||
                           (tokens[i]->type == MathExpressionToken::FUNC_CALL));
                    if (!ret)
                    {
                        stringstream error;
                        error << "Expression [" << token->token << "] must be followed by an operator";
                        m_last_error = error.str();
                    }
                }

                // Function call = open parenthesis
                if (ret && (token->type == MathExpressionToken::FUNC_CALL))
                {
                    parentheses_count++;
                }
                break;
            }

            case MathExpressionToken::OPERATOR:
            {
                // Update parentheses count
                if (token->token == "(")
                {
                    parentheses_count++;
                }
                else if (token->token == ")")
                {
                    parentheses_count--;
                }
                else if (token->token == ",")
                {
                    // Ignore function parameter separator
                }
                else
                {
                    // Binary operator
                    if (i != 0)
                    {
                        // Previous token must not be an operator, except for closing parenthesis operator
                        ret = ((tokens[i - 1]->type != MathExpressionToken::OPERATOR) ||
                               (tokens[i - 1]->token == ")"));
                        if (!ret)
                        {
                            stringstream error;
                            error << "Operator [" << token->token << "] must be preceded by a valid operand";
                            m_last_error = error.str();
                        }
                    }
                    if (i != (tokens.size() - 1))
                    {
                        // Next token must be not an operator, except for opening parenthesis operator
                        ret = ((tokens[i + 1]->type != MathExpressionToken::OPERATOR) ||
                               (tokens[i + 1]->token == "("));
                        if (!ret)
                        {
                            stringstream error;
                            error << "Operator [" << token->token << "] must be followed by a valid operand";
                            m_last_error = error.str();
                        }
                    }
                }
                break;
            }

            default:
            {
                ret = false;
                stringstream error;
                error << "Invalid token type (" << static_cast<int>(token->type) << ") for token [" << token->token << "]";
                m_last_error = error.str();
                break;
            }
        }
    }
    if (ret)
    {
        // Check parentheses
        if (parentheses_count < 0)
        {
            ret = false;
            stringstream error;
            error << "Missing " << (-1 * parentheses_count) << " opening parenthese(s)";
            m_last_error = error.str();
        }
    }

    return ret;
}

/** \brief Recursively parse a mathematical expression */
bool MathExpressionParser::parseExpression(const std::vector<MathExpressionToken*> tokens, size_t& index, bool need_ending_parenthesis, IMathExpression*& math_expr)
{
    bool ret = true;
    deque<MathExpressionToken*> exprs_tokens;
    deque<IMathExpression*> exprs;
    math_expr = nullptr;

    while (ret &&
           (index != tokens.size()) &&
           (tokens[index]->token != ")"))
    {
        MathExpressionToken* token = tokens[index];
        exprs_tokens.push_back(token);
        if (token->token == "(")
        {
            // Expression contained between parentheses
            index++;
            ret = parseExpression(tokens, index, true, math_expr);
            if (ret)
            {
                if (math_expr == nullptr)
                {
                    m_last_error = "Empty expression between parenthesis";
                }
                else
                {
                    exprs.push_back(math_expr);
                }
            }
        }
        else if (token->type == MathExpressionToken::FUNC_CALL)
        {
            // Look for the function in built-in and user functions
            FunctionCallExpression* func_expr = nullptr;
            ret = findFunction(token->token, func_expr);
            if (!ret)
            {
                stringstream error;
                error << "Unknown function [" << token->token << "]";
                m_last_error = error.str();
            }
            else
            {
                // Parse parameters
                vector<IMathExpression*> params;
                index++;
                ret = parseFunctionParams(token->token, tokens, index, params);
                if (ret)
                {
                    // Set parameters
                    if (!func_expr->setParams(params))
                    {
                        stringstream error;
                        error << "Invalid number of parameters for function [" << token->token << "]";
                        m_last_error = error.str();
                    }
                    else
                    {
                        exprs.push_back(func_expr);
                    }
                }
                if (!ret)
                {
                    // Release memory
                    delete_all(params);
                    delete func_expr;
                }
            }
        }
        else if (token->type == MathExpressionToken::CONST_EXPR)
        {
            // Numerical constant value
            double value = strtod(token->token.c_str(), nullptr);
            ConstExpression* const_expr = new ConstExpression(value);
            exprs.push_back(const_expr);
        }
        else if (token->type == MathExpressionToken::VAR_EXPR)
        {
            // Variable
            VarExpression* var_expr = new VarExpression(token->token);
            exprs.push_back(var_expr);
        }
        else if (token->type == MathExpressionToken::OPERATOR)
        {
            // Binary operator
            char op = token->token[0];
            switch (op)
            {
                case '+':
                {
                    AddOperator* op = new AddOperator();
                    exprs.push_back(op);
                    break;
                }
                case '-':
                {
                    SubsOperator* op = new SubsOperator();
                    exprs.push_back(op);
                    break;
                }
                case '*':
                {
                    MulOperator* op = new MulOperator();
                    exprs.push_back(op);
                    break;
                }
                case '/':
                {
                    DivOperator* op = new DivOperator();
                    exprs.push_back(op);
                    break;
                }
                default:
                {
                    stringstream error;
                    error << "Invalid operator : " << token->token;
                    m_last_error = error.str();
                    ret = false;
                    break;
                }
            }
        }
        if (index != tokens.size())
        {
            index++;
        }
    }
    if (ret)
    {
        // Check presence of parenthesis
        if (need_ending_parenthesis)
        {
            if ((index == tokens.size()) ||
                (tokens[index]->token != ")"))
            {
                m_last_error = "Missing closing parenthesis";
                ret = false;
            }
        }
        else
        {
            if (index != tokens.size())
            {
                m_last_error = "Missing opening parenthesis";
                ret = false;
            }
        }
    }
    if (ret)
    {
        // Factorize expression to obtain a unique mathematical expression object
        // which holds the whole mathematical expression object tree
        ret = factorizeExpression(exprs_tokens, exprs, math_expr);
    }

    if (!ret)
    {
        // Release memory
        delete_all(exprs);
    }

    return ret;
}

/** \brief Factorize expression to obtain a unique mathematical expression object which holds the while mathematical expression object tree */
bool MathExpressionParser::factorizeExpression(std::deque<MathExpressionToken*>& exprs_tokens, std::deque<IMathExpression*>exprs, IMathExpression*& math_expr)
{
    bool ret = true;
    math_expr = nullptr;

    // Check empty expression
    if (exprs_tokens.size() != 0)
    {
        // Factorize expressions around prioritary operators
        for (size_t i = 0; ret && (i < exprs_tokens.size()); i++)
        {
            if (m_prioritary_operators.find(exprs_tokens[i]->token) != m_prioritary_operators.cend())
            {
                BinaryOperator* bin_op = dynamic_cast<BinaryOperator*>(exprs[i]);
                if ((bin_op == nullptr) ||
                    (i == 0) ||
                    (i == (exprs_tokens.size() - 1u)))
                {
                    m_last_error = "Unexpected error during factorization";
                    ret = false;
                }
                else
                {
                    bin_op->setLeftExpr(exprs[i - 1]);
                    bin_op->setRightExpr(exprs[i + 1]);

                    exprs.erase(exprs.begin() + (i - 1));
                    exprs.erase(exprs.begin() + i);

                    exprs_tokens.erase(exprs_tokens.begin() + (i - 1));
                    exprs_tokens.erase(exprs_tokens.begin() + i);

                    i--;
                }
            }
        }

        // Factorize expression around not prioriatary operators
        for (size_t i = 0; ret && (i < exprs_tokens.size()); i++)
        {
            if ((exprs_tokens[i]->type == MathExpressionToken::OPERATOR) &&
                (m_prioritary_operators.find(exprs_tokens[i]->token) == m_prioritary_operators.cend()) &&
                (exprs_tokens[i]->token != "("))
            {
                BinaryOperator* bin_op = dynamic_cast<BinaryOperator*>(exprs[i]);
                if ((bin_op == nullptr) ||
                    (i == 0) ||
                    (i == (exprs_tokens.size() - 1u)))
                {
                    m_last_error = "Unexpected error during factorization";
                    ret = false;
                }
                else
                {
                    bin_op->setLeftExpr(exprs[i - 1]);
                    bin_op->setRightExpr(exprs[i + 1]);

                    exprs.erase(exprs.begin() + (i - 1));
                    exprs.erase(exprs.begin() + i);

                    exprs_tokens.erase(exprs_tokens.begin() + (i - 1));
                    exprs_tokens.erase(exprs_tokens.begin() + i);

                    i--;
                }
            }
        }

        // Extract result
        if (ret)
        {
            if (exprs.size() == 1u)
            {
                math_expr = exprs[0];
            }
            else
            {
                m_last_error = "Factorization failed to reduced to 1 mathematical expression object";
                ret = false;
            }
        }
    }

    return ret;
}

/** \brief Parse the parameters of a function call in a mathematical expression */
bool MathExpressionParser::parseFunctionParams(const std::string& function, const std::vector<MathExpressionToken*>& tokens, size_t& index, std::vector<IMathExpression*>& params)
{
    bool ret = true;
    int parentheses_count = 1;
    size_t start_of_params = index;

    // Count parentheses opening/closing until finding either the parameter separator ',' or the end of the function call
    while (ret &&
           (index != tokens.size()) &&
           (parentheses_count != 0))
    {
        MathExpressionToken* token = tokens[index];
        if (token->token == "(")
        {
            parentheses_count++;
        }
        else if (token->type == MathExpressionToken::FUNC_CALL)
        {
            parentheses_count++;
        }
        else if (token->token == ")")
        {
            parentheses_count--;
        }
        else if ((token->token == ",") &&
                 (parentheses_count == 1))
        {
            // Parse recursively the mathematical expression corresponding to the parameter
            IMathExpression* math_expr = nullptr;
            vector<MathExpressionToken*> params_tokens;
            for (size_t i = start_of_params; i < index; i++)
            {
                params_tokens.push_back(tokens[i]);
            }
            size_t params_index = 0;
            ret = parseExpression(params_tokens, params_index, false, math_expr);
            if (ret)
            {
                if (math_expr == nullptr)
                {
                    stringstream error;
                    error << "Empty parameter in parameter list for function [" << function << "]";
                    m_last_error = error.str();
                    ret = false;
                }
                else
                {
                    params.push_back(math_expr);
                    start_of_params = index + 1;
                }
            }
        }
        if (index != tokens.size())
        {
            index++;
        }
    }
    if (ret)
    {
        if (parentheses_count != 0)
        {
            stringstream error;
            error << "Missing closing parenthesis for function [" << function << "]";
            m_last_error = error.str();
            ret = false;
        }
        else
        {
            // Add last parameter
            IMathExpression* math_expr = nullptr;
            vector<MathExpressionToken*> params_tokens;
            for (size_t i = start_of_params; i < (index - 1); i++)
            {
                params_tokens.push_back(tokens[i]);
            }
            size_t params_index = 0;
            ret = parseExpression(params_tokens, params_index, false, math_expr);
            if (ret)
            {
                if (math_expr == nullptr)
                {
                    if (params.size() != 0)
                    {
                        stringstream error;
                        error << "Empty parameter in parameter list for function [" << function << "]";
                        m_last_error = error.str();
                        ret = false;
                    }
                }
                else
                {
                    params.push_back(math_expr);
                    if (index != tokens.size())
                    {
                        index--;
                    }
                }
            }
        }
    }

    return ret;
}

/** \brief Look for a function in built-in and user functions */
bool MathExpressionParser::findFunction(const std::string& function_name, FunctionCallExpression*& func_expr)
{
    bool found = false;

    // Look into user functions
    auto iter = m_user_functions.find(function_name);
    if (iter != m_user_functions.end())
    {
        func_expr = iter->second->clone();
        found = true;
    }
    else
    {
        // Look into built-in functions
        auto iter2 = m_built_in_functions.find(function_name);
        if (iter2 != m_built_in_functions.end())
        {
            func_expr = iter2->second->clone();
            found = true;
        }
    }

    return found;
}
