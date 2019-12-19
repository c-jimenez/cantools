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

#ifndef MATHEXPRESSION_H
#define MATHEXPRESSION_H

#include "VectorHelper.h"

#include <map>
#include <vector>
#include <string>
#include <functional>


/** \brief Interface for all mathematical expression implementations */
class IMathExpression
{
    public:

        /** \brief Destructor */
        virtual ~IMathExpression() {};


        /** \brief Environment data for mathematical expression's evaluation */
        typedef std::map<std::string, double> MathExpressionEnv;


        /** \brief Evaluate the mathematical expression's value */
        virtual double evaluate(const MathExpressionEnv& var_values) = 0;
};

/** \brief Variable in a mathematical expression */
class VarExpression : public IMathExpression
{
    public:

        /** \brief Constructor */
        VarExpression(const std::string& var_name)
        : m_var_name(var_name)
        {}

        /** \brief Destructor */
        virtual ~VarExpression() {};

        /** \brief Evaluate the mathematical expression's value */
        virtual double evaluate(const MathExpressionEnv& var_values) override { return var_values.at(m_var_name); }


    private:

        /** \brief Name of the variable */
        const std::string m_var_name;
};

/** \brief Numerical constant in a mathematical expression */
class ConstExpression : public IMathExpression
{
    public:

        /** \brief Constructor */
        ConstExpression(double value)
        : m_value(value)
        {}

        /** \brief Destructor */
        virtual ~ConstExpression() {};

        /** \brief Evaluate the mathematical expression's value */
        virtual double evaluate(const MathExpressionEnv& var_values) override { return m_value; }

    private:

        /** \brief Numerical constant's value */
        double m_value;
};


/** \brief Base class for binary operators in a mathematical expression */
class BinaryOperator : public IMathExpression
{
    public:

        /** \brief Constructor */
        BinaryOperator()
        : m_left_expr(nullptr)
        , m_right_expr(nullptr)
        {}

        /** \brief Destructor */
        virtual ~BinaryOperator()
        {
            delete m_left_expr;
            delete m_right_expr;
        };


        /** \brief Evaluate the mathematical expression's value */
        virtual double evaluate(const MathExpressionEnv& var_values)
        {
            double left_expr = m_left_expr->evaluate(var_values);
            double right_expr = m_right_expr->evaluate(var_values);
            return compute(left_expr, right_expr);
        }

        /** \brief Set the left operand */
        void setLeftExpr(IMathExpression* left_expr) { m_left_expr = left_expr; }

        /** \brief Set the right operand */
        void setRightExpr(IMathExpression* right_expr) { m_right_expr = right_expr; }


    protected:

        /** \brief Compute the binary operation */
        virtual double compute(double left_expr, double right_expr) = 0;


    private:

        /** \brief Left operand */
        IMathExpression* m_left_expr;

        /** \brief Right operand */
        IMathExpression* m_right_expr;
};


/** \brief Addition operator in a mathematical expression */
class AddOperator : public BinaryOperator
{
    public:

        /** \brief Constructor */
        AddOperator()
        : BinaryOperator()
        {}

        /** \brief Destructor */
        virtual ~AddOperator() {};


    protected:

        /** \brief Compute the binary operation */
        virtual double compute(double left_expr, double right_expr) override { return (left_expr + right_expr); }
};


/** \brief Substraction operator in a mathematical expression */
class SubsOperator : public BinaryOperator
{
    public:

        /** \brief Constructor */
        SubsOperator()
        : BinaryOperator()
        {}

        /** \brief Destructor */
        virtual ~SubsOperator() {};


    protected:

        /** \brief Compute the binary operation */
        virtual double compute(double left_expr, double right_expr) override { return (left_expr - right_expr); }
};


/** \brief Multiplication operator in a mathematical expression */
class MulOperator : public BinaryOperator
{
    public:

        /** \brief Constructor */
        MulOperator()
        : BinaryOperator()
        {}

        /** \brief Destructor */
        virtual ~MulOperator() {};


    protected:

        /** \brief Compute the binary operation */
        virtual double compute(double left_expr, double right_expr) override { return (left_expr * right_expr); }
};


/** \brief Division operator in a mathematical expression */
class DivOperator : public BinaryOperator
{
    public:

        /** \brief Constructor */
        DivOperator()
        : BinaryOperator()
        {}

        /** \brief Destructor */
        virtual ~DivOperator() {};


    protected:

        /** \brief Compute the binary operation */
        virtual double compute(double left_expr, double right_expr) override { return (left_expr / right_expr); }
};


/** \brief Base class for function calls in a mathematical expression */
class FunctionCallExpression : public IMathExpression
{
    public:

        /** \brief Constructor */
        FunctionCallExpression(const size_t nb_params)
        : m_nb_params(nb_params)
        {}

        /** \brief Destructor */
        virtual ~FunctionCallExpression()
        {
            delete_all(m_params);
        };


        /** \brief Set the parameter's list for the function call */
        bool setParams(const std::vector<IMathExpression*>& params)
        {
            bool ret = false;
            if (params.size() == m_nb_params)
            {
                m_params = params;
                ret = true;
            }
            return ret;
        }

        /** \brief Clone the function call's object */
        virtual FunctionCallExpression* clone() const = 0;


    protected:

        /** \brief Get the parameters list */
        const std::vector<IMathExpression*>& params() { return m_params; }


    private:

        /** \brief Number of parameters */
        const size_t m_nb_params;

        /** \brief Parameters list */
        std::vector<IMathExpression*> m_params;
};


/** \brief Function call without parameters in a mathematical expression */
class FunctionCall0Expression : public FunctionCallExpression
{
    public:

        /** \brief Constructor */
        FunctionCall0Expression(const std::function<double()>& func)
        : FunctionCallExpression(0u)
        , m_func(func)
        {}

        /** \brief Destructor */
        virtual ~FunctionCall0Expression() {};

        /** \brief Clone the function call's object */
        virtual FunctionCallExpression* clone() const override { return new FunctionCall0Expression(m_func); }

        /** \brief Evaluate the mathematical expression's value */
        virtual double evaluate(const MathExpressionEnv& var_values) override { return m_func(); }


    private:

        /** \brief Function to call */
        const std::function<double()> m_func;
};


/** \brief Function call with 1 parameter in a mathematical expression */
class FunctionCall1Expression : public FunctionCallExpression
{
    public:

        /** \brief Constructor */
        FunctionCall1Expression(const std::function<double(double)>& func)
        : FunctionCallExpression(1u)
        , m_func(func)
        {}

        /** \brief Destructor */
        virtual ~FunctionCall1Expression() {};

        /** \brief Clone the function call's object */
        virtual FunctionCallExpression* clone() const override { return new FunctionCall1Expression(m_func); }

        /** \brief Evaluate the mathematical expression's value */
        virtual double evaluate(const MathExpressionEnv& var_values) override { return m_func(params()[0]->evaluate(var_values)); }


    private:

        /** \brief Function to call */
        const std::function<double(double)> m_func;
};


/** \brief Function call with 2 parameters in a mathematical expression */
class FunctionCall2Expression : public FunctionCallExpression
{
    public:

        /** \brief Constructor */
        FunctionCall2Expression(const std::function<double(double, double)>& func)
        : FunctionCallExpression(2u)
        , m_func(func)
        {}

        /** \brief Destructor */
        virtual ~FunctionCall2Expression() {};

        /** \brief Clone the function call's object */
        virtual FunctionCallExpression* clone() const override { return new FunctionCall2Expression(m_func); }

        /** \brief Evaluate the mathematical expression's value */
        virtual double evaluate(const MathExpressionEnv& var_values) override 
        { 
            return m_func(params()[0]->evaluate(var_values),
                          params()[1]->evaluate(var_values)); 
        }


    private:

        /** \brief Function to call */
        const std::function<double(double, double)> m_func;
};


/** \brief Function call with 3 parameters in a mathematical expression */
class FunctionCall3Expression : public FunctionCallExpression
{
    public:

        /** \brief Constructor */
        FunctionCall3Expression(const std::function<double(double, double, double)>& func)
        : FunctionCallExpression(3u)
        , m_func(func)
        {}

        /** \brief Destructor */
        virtual ~FunctionCall3Expression() {};

        /** \brief Clone the function call's object */
        virtual FunctionCallExpression* clone() const override { return new FunctionCall3Expression(m_func); }

        /** \brief Evaluate the mathematical expression's value */
        virtual double evaluate(const MathExpressionEnv& var_values) override
        {
            return m_func(params()[0]->evaluate(var_values),
                          params()[1]->evaluate(var_values),
                          params()[2]->evaluate(var_values));
        }


    private:

        /** \brief Function to call */
        const std::function<double(double, double, double)> m_func;
};


/** \brief Function call with 4 parameters in a mathematical expression */
class FunctionCall4Expression : public FunctionCallExpression
{
    public:

        /** \brief Constructor */
        FunctionCall4Expression(const std::function<double(double, double, double, double)>& func)
        : FunctionCallExpression(4u)
        , m_func(func)
        {}

        /** \brief Destructor */
        virtual ~FunctionCall4Expression() {};

        /** \brief Clone the function call's object */
        virtual FunctionCallExpression* clone() const override { return new FunctionCall4Expression(m_func); }

        /** \brief Evaluate the mathematical expression's value */
        virtual double evaluate(const MathExpressionEnv& var_values) override
        {
            return m_func(params()[0]->evaluate(var_values),
                          params()[1]->evaluate(var_values),
                          params()[2]->evaluate(var_values),
                          params()[3]->evaluate(var_values));
        }


    private:

        /** \brief Function to call */
        const std::function<double(double, double, double, double)> m_func;
};


/** \brief Helper function to bind function calls without parameters */
#define FUNC_CALL_EXPR_BIND0(function)    std::bind<double, double(&)(void)>(function)

/** \brief Helper function to bind function calls with 1 parameter */
#define FUNC_CALL_EXPR_BIND1(function)    std::bind<double(&)(double), const decltype(std::placeholders::_1)&>(function, std::placeholders::_1)

/** \brief Helper function to bind function calls with 2 parameters */
#define FUNC_CALL_EXPR_BIND2(function)    std::bind<double(&)(double, double), const decltype(std::placeholders::_1)&, const decltype(std::placeholders::_2)&>(function, std::placeholders::_1, std::placeholders::_2)

/** \brief Helper function to bind function calls with 3 parameters */
#define FUNC_CALL_EXPR_BIND3(function)    std::bind<double(&)(double, double, double), const decltype(std::placeholders::_1)&, const decltype(std::placeholders::_2)&, const decltype(std::placeholders::_3)&>(function, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

/** \brief Helper function to bind function calls with 4 parameters */
#define FUNC_CALL_EXPR_BIND4(function)    std::bind<double(&)(double, double, double, double), const decltype(std::placeholders::_1)&, const decltype(std::placeholders::_2)&, const decltype(std::placeholders::_3)&, const decltype(std::placeholders::_4)&>(function, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)



#endif // MATHEXPRESSION_H
