#ifndef DIFFERETIATORH
#define DIFFERETIATORH

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <math.h>

//============================================================

#include "libr/Onegin.h"
#include "libr/Stack.h"
#include "libr/Guard.h"
#include "libr/Tree.h"

//============================================================

#define ACCURATE_CALCULATION
#define INPUT_VAR

//============================================================
 
#define PRINT_RANDOM_SENTENSE() fprintf(file, "\n%s\n", (text->lines)[(size_t) rand() % text->n_lines].str);
#define PRINT_EVERY_TIME()  Equation_tex_print_derivate(tree, print, current_node, var, file, text);

//============================================================

const int MAX_SIZE_STR            = 200;
const int MAX_SIZE_COMMAND        = 400;
const int MAX_SIZE_NAME_VARIABLES = 20;
const int MAX_SIZE_NAME_FUNC      = 10;

const int INITIAL_VAR = 20;

const double ACCURACY = 1E-7;

//============================================================

#define FUNCTION_CONSTRUCT(func)   \
    (func)->name_function = #func; \
    Function_construct(func)

#define strlen_n(s) ((s) == nullptr ? 0       : strlen(s))
#define strdup_n(s) ((s) == nullptr ? nullptr : strdup(s))

#define PREV_OP(node)   (((node)->prev) == nullptr ? 0 : (char)(node)->prev->value)
#define PREV_TYPE(node) (((node)->prev) == nullptr ? 0 : (char)(node)->prev->type)

//============================================================

struct Equation 
{
    char* buffer;
    char* current_symbol;
};

struct Function
{   
    const char* name_function;
          char* name_equation;

    struct Tree* tree;

    struct Stack* vars;
    struct Stack* derivates;
};

struct Variable
{
    char* name;
    unsigned int hash;

    double value;
};

//======================================================================

enum NODE_TYPE 
{
    NUMBER    = 1,
    VAR       = 2,
    OPERATION = 3,
    FUNC      = 4,
};

enum OPERATION
{
    #define DEF_OP(operator, OP, number, code) OP = number,
    #define DEF_FUNC(func, FUNC, hash, number, code) 

    #include "commands.h"

    #undef DEF_OP
    #undef DEF_FUNC
};

enum FUNC
{
    #define DEF_OP(opetation, OP, number, code) 
    #define DEF_FUNC(func, FUNC, hash, number, code) FUNC = number, 
    
    #include "commands.h"

    #undef DEF_OP
    #undef DEF_FUNC
};


enum PRINT
{
    PRINT_VAR    = 0,
    PRINT_NUMBER = 1,  
};

//======================================================================

void Function_construct(struct Function* function);

void Function_create(struct Function* function, const char* name_function);

struct Node* Node_GetG(struct Tree* tree, struct Equation* equation, char* line, struct Stack* vars);

struct Node* Node_GetE(struct Tree* tree, struct Node* previous_node, struct Equation* equation, struct Stack* vars);

struct Node* Node_GetT(struct Tree* tree, struct Node* previous_node, struct Equation* equation, struct Stack* vars);

struct Node* Node_GetD(struct Tree* tree, struct Node* previous_node, struct Equation* equation, struct Stack* vars);

struct Node* Node_GetP(struct Tree* tree, struct Node* previous_node, struct Equation* equation, struct Stack* vars);

struct Node* Node_GetN(struct Tree* tree, struct Node* previous_node, struct Equation* equation);

struct Node* Node_GetW(struct Tree* tree, struct Node* previous_node, struct Equation* equation, struct Stack* vars);

void Require(struct Tree* tree, struct Equation* equation, char symbol);

void Syntax_error(struct Equation* equation);

//=========================================================================

void Function_destruct(struct Function* function);

void Function_stacks_destruct(struct Function* function);

//=========================================================================

void Total_derivative(struct Function* function, FILE* file, struct Text* text);

struct Node* Derivative(struct Function* function, struct Tree* tree, struct Node* current_node, char* var, 
                                                   unsigned int hash_var, FILE* file, struct Text* text);
                                                   
double Evaluation(struct Function* function, struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text);

void Value_equation_replace(struct Function* function, struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text);

void Var_equal_replace(struct Function* function, struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text);

void Constant_folding(struct Function* function, struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text);

void Neutral_delete(struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text);

void Pow_folding(struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text);

void Optimization(struct Function* function, struct Tree* tree, FILE* file, struct Text* text);

void Re_linking_subtree(struct Tree* tree, struct Node* current_node, struct Node* subtree_linking, struct Node* subtree_delete);

void Replace_func(struct Tree* tree, struct Node* current_node, double value);

//==========================================================================

size_t Skip_spaces(char** string);

int Code_func(char* func);

char* Get_name_equation(const char* name_file);

struct Variable* Find_var(struct Stack* vars, unsigned int hash);

//==========================================================================

int Subtree_is_number(struct Tree* tree, struct Node* current_node);

size_t Size_subtree(struct Tree* tree, struct Node* current_node);

void Node_count(struct Tree* tree, struct Node* current_node, size_t* count);

int Is_equal(double value, double number);

unsigned int Hash(const char* cmd);

//=======================================================================

void Tree_text_print(struct Tree* tree);

void Node_text_print(struct Tree* tree, struct Node* current_node, FILE* file);

void Equation_tex_dump(struct Tree* tree, struct Text* text);

FILE* Equation_tex_dump_open(struct Tree* tree, struct Text* text);

void Equation_tex_print(struct Tree* tree, struct Node* begin, FILE* file, struct Text* text);

void Equation_tex_print_derivate(struct Tree* tree, struct Node* before, struct Node* after, char* var, FILE* file, struct Text* text);

void Equation_tex_print_total_differential(struct Function* function, int mode, FILE* file, struct Text* text);

void Tree_tex_print(struct Tree* tree, FILE* file);

void Subtree_tex_print(struct Tree* tree, struct Node* begin, FILE* file);

void Node_tex_print(struct Tree* tree, struct Node* begin, struct Node* current_node, FILE* file);

void Equation_tex_dump_close(struct Tree* tree, FILE* file);


#endif