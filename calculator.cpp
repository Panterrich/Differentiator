#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "libr/Onegin.h"
#include "Differentiator.h"
#include "calculator.h"

//===================================================================

int main(int argc, char* argv[])
{
    FILE* file = fopen(argv[1], "r");

    struct Equation equaction = {};

    struct Text text = {};
    Create_text(file, &text);

    printf("Ответ: %lg\n", GetG(&equaction, text.lines[0].str));
}


double GetG(struct Equation* equation, char* line)
{
    equation->buffer         = line;
    equation->current_symbol = line;

    Skip_spaces(&equation->current_symbol);

    double value = GetE(equation);

    Skip_spaces(&equation->current_symbol);
    Calc_require(equation, '$');    

    return value;
}

double GetE(struct Equation* equation)
{
    double left_value = 1;

    if (*equation->current_symbol == '-') 
    {
        left_value = -1;

        ++(equation->current_symbol);
        Skip_spaces(&equation->current_symbol);
    }

    left_value *= GetT(equation);

    while ((*equation->current_symbol == '+') ||
           (*equation->current_symbol == '-'))
    {
        int op = *equation->current_symbol;

        ++(equation->current_symbol);
        Skip_spaces(&equation->current_symbol);

        double right_value = GetT(equation);

        if (op == '+') left_value += right_value;
        else           left_value -= right_value;
    }

    return left_value;
}

double GetT(struct Equation* equation)
{
    double left_value = GetD(equation);

    while ((*equation->current_symbol == '*') ||
           (*equation->current_symbol == '/'))
    {
        int op = *equation->current_symbol;

        ++(equation->current_symbol);
        Skip_spaces(&equation->current_symbol);

        double right_value = GetD(equation);

        if (op == '*') left_value *= right_value;
        else           left_value /= right_value;
    }

    return left_value;
}

double GetD(struct Equation* equation)
{
    double left_value = GetP(equation);

    while (*equation->current_symbol == '^')
    {
        int op = *equation->current_symbol;

        ++(equation->current_symbol);
        Skip_spaces(&equation->current_symbol);


        double right_value = GetD(equation);

        left_value = pow(left_value, right_value);
    }

    return left_value;
}


double GetP(struct Equation* equation)
{
    if (*equation->current_symbol == '(')
    {
        ++(equation->current_symbol);
        Skip_spaces(&equation->current_symbol);

        double value = GetE(equation);

        Calc_require(equation, ')');
        Skip_spaces(&equation->current_symbol);

        return value;
    }

    else return GetW(equation);
}

double GetN(struct Equation* equation)
{
    double value = 0;
    char* begin = equation->current_symbol;

    value = strtod(equation->current_symbol, &equation->current_symbol);
    Skip_spaces(&equation->current_symbol);

    if (equation->current_symbol == begin) Syntax_error(equation);

    return value;
}

double GetW(struct Equation* equation)
{
    if (isdigit(*equation->current_symbol)) return GetN(equation);

    else
    {
        if (!isalpha(*equation->current_symbol))
        {
            Syntax_error(equation);
            return -1;
        }

        char word[MAX_WORD_SIZE] = "";
        size_t index = 0;

        while(isalpha(*equation->current_symbol) || isdigit(*equation->current_symbol) || (*equation->current_symbol == '_'))
        {
            word[index] = *equation->current_symbol;

            ++index;
            ++(equation->current_symbol);
        }

        if (*equation->current_symbol == '(')
        {   
        
            if (strcmp(word, "sin") == 0) return sin(GetP(equation));
            if (strcmp(word, "cos") == 0) return cos(GetP(equation));

            else
            {
                Syntax_error(equation);
                return -1;   
            }
        }

        else
        {
            double value = 0;

            printf("Введите значение для переменной %s: ", word);
            scanf("%lg", &value);

            return value;
        }   
    }
}

void Calc_require(struct Equation* equation, char symbol)
{
    if (*equation->current_symbol == symbol) ++(equation->current_symbol);
    else 
    {
        printf("Require symbol: %c \n", *equation->current_symbol);
        Syntax_error(equation);
    }
}