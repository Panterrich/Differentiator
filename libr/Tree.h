#ifndef TREEH
#define TREEH

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//===================================================================================================

#define TREE_ASSERT_OK(tree)                                                                        \
    if (Tree_ERROR(tree))                                                                           \
    {                                                                                               \
        FILE *log = fopen("log.txt", "a");                                                          \
        assert(log != nullptr);                                                                     \
                                                                                                    \
        fprintf(log, "\n\nERROR: file %s line %d function %s\n", __FILE__, __LINE__, __FUNCTION__); \
        Tree_dump(tree, log);                                                                       \
        fflush(log);                                                                                \
                                                                                                    \
        if (tree->root != nullptr)                                                                  \
        {                                                                                           \
            Tree_graph(tree);                                                                       \
        }                                                                                           \
                                                                                                    \
        abort();                                                                                    \
    }

#define TREE_CONSTRUCT(tree)      \
    (tree)->name_tree = #tree;    \
    Tree_construct(tree)

#define case_of_switch(enum_const) \
    case enum_const:               \
        return #enum_const;

//====================================================================================================

struct Tree
{
    const char* name_tree;
          char* name_equation;

    size_t size;
    struct Node* root;

    int error;
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

//========================================================================================================

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
    NUMBER_SYNTAX_ERROR            = 16,
    REQUIRE_SYNTAX_ERROR           = 17,
};

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

struct Tree* Tree_clone(struct Tree* old_tree);

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

#endif