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

//============================================================

#define RESIZE

#ifdef RESIZE

#define BEGIN_AND_RESIZE_FORMULA fprintf(file, "\n\n\\begin{equation}\n"             \
                                               "\\resizebox{.9\\textwidth}{!}{\n"    \
                                               "$\\displaystyle ");   

#else

#define BEGIN_AND_RESIZE_FORMULA fprintf(file, "\n\n\\begin{equation}\n"             \
                                               "\\resizebox{!}{!}{\n"                \
                                               "$\\displaystyle ");    

#endif
//============================================================
 
#define PRINT_RANDOM_SENTENSE fprintf(file, "\n%s\n", (text->lines)[rand() % text->n_lines].str);
#define PRINT_EVERY_TIME Equation_tex_print(tree, file, text);

//============================================================

const int MAX_SIZE_STR            = 200;
const int MAX_SIZE_COMMAND        = 300;
const int MAX_VARIBLES            = 20;
const int MAX_SIZE_NAME_VARIABLES = 20;
const int MAX_SIZE_NAME_FUNC      = 10;

const double ACCURACY = 1E-7;

//============================================================

#define TREE_ASSERT_OK(tree)                                                                    \
    if (Tree_ERROR(tree))                                                                       \
    {                                                                                           \
        FILE *log = fopen("log.txt", "a");                                                      \
        assert(log != nullptr);                                                                 \
                                                                                                \
        fprintf(log, "ERROR: file %s line %d function %s\n", __FILE__, __LINE__, __FUNCTION__); \
        Tree_dump(tree, log);                                                                   \
        fflush(log);                                                                            \
                                                                                                \
        if (tree->root != nullptr)                                                              \
        {                                                                                       \
            Tree_graph(tree);                                                                   \
        }                                                                                       \
                                                                                                \
        abort();                                                                                \
    }

#define TREE_CONSTRUCT(tree)      \
    (tree)->name_tree = #tree;    \
    Tree_construct(tree)

#define case_of_switch(enum_const) \
    case enum_const:               \
        return #enum_const;

#define strlen_n(s) ((s) == nullptr ? 0       : strlen(s))
#define strdup_n(s) ((s) == nullptr ? nullptr : strdup(s))

#define PREV_OP(node)   (((node)->prev) == nullptr ? 0 : (char)(node)->prev->value)
#define PREV_TYPE(node) (((node)->prev) == nullptr ? 0 : (char)(node)->prev->type)
//============================================================

struct Variable
{
    char* name;
    unsigned int hash;

    double value;
};

struct Node
{
    int type;

    double value;
    char* str;

    struct Node* prev;

    struct Node* left;
    struct Node* right;
};

struct Tree
{
    const char* name_tree;
          char* name_equation;
    
    struct Variable* name_table;

    size_t size;
    struct Node* root;

    int error;
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
//======================================================================

enum TREE_ERROR
{
    TREE_NEGATIVE_SIZE             = 1,
    TREE_WRONG_SIZE                = 2,
    
    TREE_WRONG_NODE_TYPE           = 3,
    TREE_WRONG_NODE_VALUE          = 4,
    TREE_WRONG_NODE_STR            = 5,
    TREE_WRONG_FUNC_LEFT_CHILD     = 6,
    TREE_WRONG_VAR_CHILD           = 7,
    TREE_WRONG_NUMBER_CHILD        = 8,

    TREE_WRONG_NAME_DATA_EQUATION  = 10,

    TREE_WRONG_FUNC                = 11,

    EQUATION_SYNTAX_ERROR          = 12,
    FUNC_SYNTAX_ERROR              = 13,
    TYPE_SYNTAX_ERROR              = 14,
    OPERATOR_SYNTAX_ERROR          = 15,
    VAR_OVERFLOW_ERROR             = 16,
};

//======================================================================

void Tree_create(struct Tree* tree, const char* name_equation);

struct Node* Node_create(struct Tree* tree, struct Node* previos_node, char** current_operation);

void Tree_text_print(struct Tree* tree);

void Node_text_print(struct Tree* tree, struct Node* current_node, FILE* file);

unsigned int Hash(const char* cmd);

void Equation_tex_dump(struct Tree* tree, struct Text* text);

FILE* Equation_tex_dump_open(struct Tree* tree, struct Text* text);

void Equation_tex_print(struct Tree* tree, FILE* file, struct Text* text);

void Node_tex_print(struct Tree* tree, struct Node* current_node, FILE* file);

void Equation_tex_dump_close(struct Tree* tree, FILE* file);

size_t Skip_spaces(char** string);

char* Get_name_func(char** pointer_to_name_func);

int Code_func(char* func);

int Get_operator(char** current_operator);

char* Get_name_equation(const char* name_file);

int Subtree_is_number(struct Tree* tree, struct Node* current_node);

size_t Size_subtree(struct Tree* tree, struct Node* current_node);

void Node_count(struct Tree* tree, struct Node* current_node, size_t* count);

int Find_func(char* current_symbol);

struct Variable* Find_var(struct Tree* tree, unsigned int hash);

int Is_equal(double value, double number);

struct Node* Derivative(struct Tree* tree, struct Node* current_node, char* var, unsigned int hash_var, FILE* file, struct Text* text);

double Evaluation(struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text);

void Constant_folding(struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text);

void Neutral_delete(struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text);

void Optimization(struct Tree* tree, FILE* file, struct Text* text);

void Re_linking_subtree(struct Tree* tree, struct Node* current_node, struct Node* subtree_linking, struct Node* subtree_delete);

//=============================================================================================

void Tree_construct(struct Tree* tree);

void Node_fill(struct Node* node,  
               struct Node* left_node, 
               int type, char* string, double value, struct Node* previous_node, 
               struct Node* right_node);

struct Node* Node_create_and_fill(struct Node* left_node, 
                                  int type, char* string, double value, struct Node* previous_node, 
                                  struct Node* right_node);
               
void Tree_destruct(struct Tree* tree);

void Subtree_destruct(struct Node* current_node);

void Node_destruct(struct Node* current_node);

struct Node* Node_clone(struct Node* current_node_for_clone, struct Node* previous_node_clone);

struct Stack* Tree_search(const char* name, struct Tree* tree);

int Node_search(const char* name, struct Node* current_node, struct Stack* path_element);


void Tree_null_check(struct Tree* tree);

int Tree_ERROR(struct Tree* tree);

void Node_validator(struct Tree* tree, struct Node* current_node, size_t* count);

void Tree_dump(struct Tree* tree, FILE* log);

void Print_node(struct Tree* tree, struct Node* current_node, size_t* count, FILE* log);

void Tree_graph(struct Tree* tree);

void Node_graph(struct Tree* tree, struct Node* current_node, size_t* count, FILE* text);

const char* Tree_text_ERROR(struct Tree* tree);
