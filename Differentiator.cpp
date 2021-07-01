#include "Differentiator.h"
#include "DSL.h"

void Function_construct(struct Function* function)
{
    assert(function != nullptr);

    function->tree       = (struct Tree*)  calloc(1, sizeof(struct Stack));

    function->vars       = (struct Stack*) calloc(1, sizeof(struct Stack));
    function->derivates  = (struct Stack*) calloc(1, sizeof(struct Stack));
    
    STACK_CONSTRUCT(function->vars, INITIAL_VAR);
    STACK_CONSTRUCT(function->derivates, INITIAL_VAR);

    TREE_CONSTRUCT(function->tree);
}

void Function_create(struct Function* function, const char* name_equation)
{
    assert(function      != nullptr); 
    assert(name_equation != nullptr);

    char command[MAX_SIZE_COMMAND] = {};
    sprintf(command, "iconv -f WINDOWS-1251 -t UTF-8 %s -o test.txt && mv test.txt %s", name_equation, name_equation);
    system(command);

    FILE* text = fopen(name_equation, "rb");

    if (text == nullptr)
    {
        function->tree->error = TREE_WRONG_NAME_DATA_EQUATION;
        TREE_ASSERT_OK(function->tree);
    }

    struct Text formula = {};
    Create_text(text, &formula);

    fclose(text);

    if (function->tree->root != nullptr)
    {
        Tree_destruct(function->tree);
    }
    
    function->name_equation       = Get_name_equation(name_equation);
    function->tree->name_equation = Get_name_equation(name_equation);
    
    struct Equation equation = {};

    Node_GetG(function->tree, &equation, formula.lines[0].str, function->vars);

    Free_memory(&formula);

    sprintf(command, "iconv --from-code=UTF-8 --to-code=WINDOWS-1251 %s -o %s", name_equation, name_equation);
    system(command);
}

struct Node* Node_GetG(struct Tree* tree, struct Equation* equation, char* line, struct Stack* vars)
{
    Tree_null_check(tree);
    assert(equation != nullptr);
    assert(vars     != nullptr);

    equation->buffer         = line;
    equation->current_symbol = line;

    Skip_spaces(&equation->current_symbol);

    tree->root = Node_GetE(tree, nullptr, equation, vars);

    Skip_spaces(&equation->current_symbol);
    Require(tree, equation, '$');    

    return tree->root;
}

struct Node* Node_GetE(struct Tree* tree, struct Node* previous_node, struct Equation* equation, struct Stack* vars)
{   
    Tree_null_check(tree);
    assert(equation != nullptr);
    assert(vars     != nullptr);

    struct Node* current_node = nullptr;

    if (*equation->current_symbol == '-') 
    {   
        ++(equation->current_symbol);
        Skip_spaces(&equation->current_symbol);

        struct Node* right = Node_GetT(tree, nullptr, equation, vars);

        current_node = Node_create_and_fill(nullptr, 
                                            OPERATION, nullptr, SUB, previous_node,
                                            right);
        ++(tree->size);
    }

    else
    {
        current_node = Node_GetT(tree, previous_node, equation, vars);
    }
    

    while ((*equation->current_symbol == '+') ||
           (*equation->current_symbol == '-'))
    {
        int op = *equation->current_symbol;

        ++(equation->current_symbol);
        Skip_spaces(&equation->current_symbol);

        struct Node* left  = current_node;
        struct Node* right = Node_GetT(tree, nullptr, equation, vars);

        current_node = Node_create_and_fill(left, 
                                            OPERATION, nullptr, op, previous_node,
                                            right);
        ++(tree->size);
    }

    return current_node;
}

struct Node* Node_GetT(struct Tree* tree, struct Node* previous_node, struct Equation* equation, struct Stack* vars)
{
    Tree_null_check(tree);
    assert(equation != nullptr);
    assert(vars     != nullptr);

    struct Node* current_node = Node_GetD(tree, previous_node, equation, vars);

    while ((*equation->current_symbol == '*') ||
           (*equation->current_symbol == '/'))
    {
        int op = *equation->current_symbol;

        ++(equation->current_symbol);
        Skip_spaces(&equation->current_symbol);

        struct Node* left  = current_node;
        struct Node* right = Node_GetD(tree, nullptr, equation, vars);

        current_node = Node_create_and_fill(left,
                                            OPERATION, nullptr, op, previous_node,
                                            right);
        ++(tree->size);
    }

    return current_node;
}

struct Node* Node_GetD(struct Tree* tree, struct Node* previous_node, struct Equation* equation, struct Stack* vars)
{
    Tree_null_check(tree);
    assert(equation != nullptr);
    assert(vars     != nullptr);

    struct Node* current_node = Node_GetP(tree, nullptr, equation, vars);

    while (*equation->current_symbol == '^')
    {
        int op = *equation->current_symbol;

        ++(equation->current_symbol);
        Skip_spaces(&equation->current_symbol);

        struct Node* left  = current_node;
        struct Node* right = Node_GetD(tree, nullptr, equation, vars);

        current_node = Node_create_and_fill(left,
                                            OPERATION, nullptr, op, previous_node,
                                            right);
        ++(tree->size);
    }

    return current_node;
}

struct Node* Node_GetP(struct Tree* tree, struct Node* previous_node, struct Equation* equation, struct Stack* vars)
{   
    Tree_null_check(tree);
    assert(equation != nullptr);
    assert(vars     != nullptr);

    if (*equation->current_symbol == '(')
    {
        ++(equation->current_symbol);
        Skip_spaces(&equation->current_symbol);

        struct Node* current_node = Node_GetE(tree, previous_node, equation, vars);

        Require(tree, equation, ')');
        Skip_spaces(&equation->current_symbol);

        return current_node;
    }

    else return Node_GetW(tree, previous_node, equation, vars);
}

struct Node* Node_GetN(struct Tree* tree, struct Node* previous_node, struct Equation* equation)
{
    Tree_null_check(tree);
    assert(equation != nullptr);

    double value = 0;
    char* begin = equation->current_symbol;

    value = strtod(equation->current_symbol, &equation->current_symbol);
    Skip_spaces(&equation->current_symbol);

    if (equation->current_symbol == begin) 
    {
        tree->error = NUMBER_SYNTAX_ERROR;
        Syntax_error(equation);
        TREE_ASSERT_OK(tree);
    }

    struct Node* current_node = Node_create_and_fill(nullptr, 
                                                     NUMBER, nullptr, value, previous_node,
                                                     nullptr);
    ++(tree->size);

    return current_node;
}

struct Node* Node_GetW(struct Tree* tree, struct Node* previous_node, struct Equation* equation, struct Stack* vars)
{
    Tree_null_check(tree);
    assert(equation != nullptr);
    assert(vars     != nullptr);

    if (isdigit(*equation->current_symbol)) return Node_GetN(tree, previous_node, equation);

    else
    {
        if (!isalpha(*equation->current_symbol))
        {
            tree->error = EQUATION_SYNTAX_ERROR;
            Syntax_error(equation);
            TREE_ASSERT_OK(tree);
        }

        char* word = (char*) calloc(MAX_SIZE_STR, sizeof(char));
        size_t index = 0;

        while(isalpha(*equation->current_symbol) || isdigit(*equation->current_symbol) || (*equation->current_symbol == '_'))
        {
            word[index] = *equation->current_symbol;

            ++index;
            ++(equation->current_symbol);
        }

        word[index] = '\0';
        Skip_spaces(&equation->current_symbol);

        if (*equation->current_symbol == '(')
        {   
            int code = Code_func(word);

            if (code == -1)
            {
                tree->error = FUNC_SYNTAX_ERROR;
                Syntax_error(equation);
                TREE_ASSERT_OK(tree);
            }

            struct Node* arg = Node_GetP(tree, nullptr, equation, vars);

            struct Node* current_node = Node_create_and_fill(nullptr,
                                                             FUNC, word, code, previous_node, 
                                                             arg);
            ++(tree->size);

            return current_node;
        }

        else
        {
            unsigned int var_hash = Hash(word);

            struct Variable* variable = Find_var(vars, var_hash);

            if (variable == nullptr)
            {
                variable = (struct Variable*) calloc(1, sizeof(struct Variable));

                variable->name  = word;
                variable->hash  = var_hash;
                variable->value = NAN;

                Stack_push(vars, variable);

                #ifdef INPUT_VAR
                    double value = 0;
                    printf("Введите значение для переменной %s: ", word);
                    scanf("%lg", &value);

                    variable->value = value;
                #endif
            }
            
            struct Node* current_node = Node_create_and_fill(nullptr,
                                                             VAR, word, variable->value, previous_node,
                                                             nullptr);
            ++(tree->size);

            return current_node;
        }   
    }
}

void Require(struct Tree* tree, struct Equation* equation, char symbol)
{
    if (*equation->current_symbol == symbol) ++(equation->current_symbol);
    else 
    {
        tree->error = REQUIRE_SYNTAX_ERROR;

        printf("Require symbol: %c \n", *equation->current_symbol);

        Syntax_error(equation);
        TREE_ASSERT_OK(tree);
    }
}

void Syntax_error(struct Equation* equation)
{
    printf("Formula: %s \n"
           "Begin syntax error: [%s] \n", 
           equation->buffer, equation->current_symbol);
}

//========================================================================================

void Function_destruct(struct Function* function)
{
    assert(function != nullptr);

    Tree_destruct(function->tree);
    Function_stacks_destruct(function);

    free(function->name_equation);

    function->name_equation = nullptr;
    function->tree          = nullptr;
}

void Function_stacks_destruct(struct Function* function)
{
    assert(function != nullptr);

    if (function->vars != nullptr)
    {
        for (size_t index = 0; index < function->vars->size; ++index) 
        {
            struct Variable* var = (struct Variable*) Stack_pop(function->vars);

            free(var->name);
            free(var);
        }

        Stack_destruct(function->vars);
        free(function->vars);
    }

    if (function->derivates)
    {
        for (size_t index = 0; index < function->derivates->size; ++index) 
        {
            struct Tree* tree = (struct Tree*) Stack_pop(function->derivates);

            Tree_destruct(tree);
            free(tree);
        }

        Stack_destruct(function->derivates);
        free(function->derivates);
    }

    function->vars      = nullptr;
    function->derivates = nullptr;
}

//========================================================================================

void Total_derivative(struct Function* function, FILE* file, struct Text* text)
{
    assert(function != nullptr);
    assert(file != nullptr);
    assert(text != nullptr);

    for (size_t count_var = 0; count_var < function->vars->size; ++count_var)
    {
        struct Tree* der_var = Tree_clone(function->tree);

        Derivative(function, der_var, der_var->root, ((struct Variable*)((function->vars->data)[count_var]))->name, 
                                                     ((struct Variable*)((function->vars->data)[count_var]))->hash, file, text);
        Optimization(function, der_var, file, text);

        Stack_push(function->derivates, der_var);
    }

    Equation_tex_print_total_differential(function, PRINT_VAR, file, text);
   

    #ifdef INPUT_VAR
        for (size_t index = 0; index < function->derivates->size; ++index)
        {   
            #ifdef ACCURATE_CALCULATION
                Var_equal_replace(function, (struct Tree*)((function->derivates->data)[index]), 
                                           ((struct Tree*)((function->derivates->data)[index]))->root, file, text);

                Optimization(function, (struct Tree*)((function->derivates->data)[index]), file, text);

            #else 
                Value_equation_replace(function, (struct Tree*)((function->derivates->data)[index]), 
                                                ((struct Tree*)((function->derivates->data)[index]))->root, file, text);
            #endif
        }

        Equation_tex_print_total_differential(function, PRINT_NUMBER, file, text);
        
    #endif
}

struct Node* Derivative(struct Function* function, struct Tree* tree, struct Node* current_node, char* var, 
                                                   unsigned int hash_var, FILE* file, struct Text* text)
{
    Tree_null_check(tree);
    assert(current_node != nullptr);
    assert(var  != nullptr);
    assert(file != nullptr);
    
    struct Node* print = Node_clone(current_node, nullptr);

    switch (TYPE)
    {
        case NUMBER: current_node->value = 0; break;
        case VAR: 
        {
            if (strcmp(current_node->str, var) == 0)
            {
                current_node->type = NUMBER;
                current_node->str = nullptr;
                current_node->value = 1;
            }

            else
            {   
                current_node->type = NUMBER;
                current_node->str = nullptr;
                current_node->value = 0;
            }

            break;
        }

        case OPERATION: 
        {
            switch (OP)
            {
                #define DEF_OP(operator, OP, number, code) case OP: code; break;
                #define DEF_FUNC(func, FUNC, hash, number, code)
                
                #include "commands.h"

                #undef DEF_OP
                #undef DEF_FUNC

                default:
                    tree->error = OPERATOR_SYNTAX_ERROR;
                    TREE_ASSERT_OK(tree);
                    break;
            }
            
            break;
        }
        
        case FUNC:
        {
            switch (OP)
            {
                #define DEF_OP(operator, OP, number, code)
                #define DEF_FUNC(func, FUNC, hash, number, code) case FUNC: code; break;
                
                #include "commands.h"

                #undef DEF_OP
                #undef DEF_FUNC

                default:
                    tree->error = FUNC_SYNTAX_ERROR;
                    TREE_ASSERT_OK(tree);
                    break;
            }

            break;        
        }

        default:
            tree->error = TYPE_SYNTAX_ERROR;
            TREE_ASSERT_OK(tree);
            break;
    }

    tree->size = Size_subtree(tree, tree->root);

    PRINT_EVERY_TIME();
    Node_destruct(print);
    
    return current_node;
}

double Evaluation(struct Function* function, struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text)
{   
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);
    assert(current_node != nullptr);

    switch (TYPE)
    {
        case NUMBER: return current_node->value;
        case VAR:    return Find_var(function->vars, Hash(current_node->str))->value;
        case OPERATION:
            switch (OP)
            {
                case ADD: return LCALC + RCALC;
                case SUB: return LCALC - RCALC;
                case MUL: return LCALC * RCALC;
                case DIV: return LCALC / RCALC;     
                case POW: return pow(LCALC, RCALC);

                default:
                    tree->error = OPERATOR_SYNTAX_ERROR;
                    TREE_ASSERT_OK(tree);
                    break;
            }
            break;

        case FUNC:
            switch (OP)
            {
                case LN:     return log(RCALC);
                case SIN:    return sin(RCALC);
                case COS:    return cos(RCALC);
                case TG:     return tan(RCALC);
                case CTG:    return 1 / tan(RCALC);
                case ARCSIN: return asin(RCALC);
                case ARCCOS: return acos(RCALC);
                case ARCTG:  return atan(RCALC);
                case ARCCTG: return M_PI_2 - atan(RCALC);
                case SH:     return sinh(RCALC);
                case CH:     return cosh(RCALC);
                case TH:     return tanh(RCALC);
                case CTH:    return 1 / tan(RCALC);
                case SQRT:   return sqrt(RCALC);
                case EXP:    return exp(RCALC);

                default:
                    tree->error = FUNC_SYNTAX_ERROR;
                    TREE_ASSERT_OK(tree);
                    break;
            }
            break;

        default:
            tree->error = TYPE_SYNTAX_ERROR;
            TREE_ASSERT_OK(tree);
            break;
    }

    return 0;
}

void Value_equation_replace(struct Function* function, struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text)
{
    double value = Evaluation(function, tree, current_node, file, text);

    if (LNODE != nullptr) Subtree_destruct(LNODE);
    if (RNODE != nullptr) Subtree_destruct(RNODE);
    if (FUNCTION != nullptr) free(FUNCTION);

    Node_fill(current_node, 
              nullptr, 
              NUMBER, nullptr, value, current_node->prev, 
              nullptr);
    
    tree->size = Size_subtree(tree, tree->root);
}

void Var_equal_replace(struct Function* function, struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text)
{
    if (LNODE != nullptr) Var_equal_replace(function, tree, LNODE, file, text);
    if (RNODE != nullptr) Var_equal_replace(function, tree, RNODE, file, text);

    if (current_node->type == VAR)
    {
        double value = Find_var(function->vars, Hash(current_node->str))->value;

        free(current_node->str); 

        Node_fill(current_node,
                  LNODE,
                  NUMBER, nullptr, value, PREV,
                  RNODE
                  );
    }
}

void Optimization(struct Function* function, struct Tree* tree, FILE* file, struct Text* text)
{
    size_t old_size = 0;
    
    while (tree->size != old_size)
    {   
        old_size = tree->size;

        Equation_tex_print(tree, tree->root, file, text);

        Constant_folding(function, tree, tree->root, file, text);
        Neutral_delete(tree, tree->root, file, text);
        Pow_folding(tree, tree->root, file, text);
    }
}

void Constant_folding(struct Function* function, struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text)
{
    if (!Subtree_is_number(tree, current_node))
    {
        if (LNODE != nullptr) Constant_folding(function, tree, LNODE, file, text);
        if (RNODE != nullptr) Constant_folding(function, tree, RNODE, file, text);
    }
    
    else
    {
        double value = Evaluation(function, tree, current_node, file, text);

        struct Node* equal = Node_create_and_fill(nullptr,
                                                  NUMBER, nullptr, value, PREV,
                                                  nullptr);
        
        if (current_node == tree->root) 
        {
            tree->root = equal;
        }

        else
        {
            if (PREV->left  == current_node) PREV->left = equal;
            if (PREV->right == current_node) PREV->right = equal;
        }

        tree->size = Size_subtree(tree, tree->root);

        Subtree_destruct(current_node);
    }
}

void Pow_folding(struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text)
{
    if ((TYPE == OPERATION) && (OP == POW) && (LNODE != nullptr))
    {
        if ((LNODE->type == OPERATION) && ((char)LNODE->value == POW))
        {
            struct Node* base      = LNODE->left;
            struct Node* left_deg  = LNODE->right;
            struct Node* right_deg = RNODE;
            struct Node* mul       = LNODE;

            Node_fill(mul, 
                      left_deg,
                      OPERATION, nullptr, MUL, current_node,
                      right_deg);

            Node_fill(current_node,
                      base, 
                      OPERATION, nullptr, POW, PREV,
                      mul);
        }
    }

    else
    {
        if (LNODE != nullptr) Pow_folding(tree, LNODE, file, text);
        if (RNODE != nullptr) Pow_folding(tree, RNODE, file, text);
    }
}

void Neutral_delete(struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text)
{
    Tree_null_check(tree);
    assert(file != nullptr);
    assert(text != nullptr);

    if (current_node != nullptr)
    {
        if (LNODE != nullptr) Neutral_delete(tree, LNODE, file, text);
        if (RNODE != nullptr) Neutral_delete(tree, RNODE, file, text);
    }
    else
    {
        return;
    }

    if (TYPE == OPERATION)
    {
        switch (OP)
        {
            case ADD:
            case SUB:
            {
                if (LNODE != nullptr)
                {
                    if ((LNODE->type == NUMBER) && (Is_equal(LNODE->value, 0)))
                    {
                        Re_linking_subtree(tree, current_node, RNODE, LNODE);
                        return;
                    }
                }

                if (RNODE != nullptr)
                {
                    if ((RNODE->type == NUMBER) && (Is_equal(RNODE->value, 0)))
                    {
                        Re_linking_subtree(tree, current_node, LNODE, RNODE);
                        return;
                    }
                }
                
                break;
            }

            case MUL:
            {
                if (LNODE != nullptr)
                {
                    if ((LNODE->type == NUMBER) && (Is_equal(LNODE->value, 0)))
                    {
                        Re_linking_subtree(tree, current_node, LNODE, RNODE);
                        return;
                    }
                }
                if (RNODE != nullptr)
                {
                    if ((RNODE->type == NUMBER) && (Is_equal(RNODE->value, 0)))
                    {
                        Re_linking_subtree(tree, current_node, RNODE, LNODE);
                        return;
                    }
                }

                if (LNODE != nullptr)
                {
                    if ((LNODE->type == NUMBER) && (Is_equal(LNODE->value, 1)))
                    {
                        Re_linking_subtree(tree, current_node, RNODE, LNODE);
                        return;
                    }
                }
                if (RNODE != nullptr)
                {
                    if ((RNODE->type == NUMBER) && (Is_equal(RNODE->value, 1)))
                    {
                        Re_linking_subtree(tree, current_node, LNODE, RNODE);
                        return;
                    }
                }

                break;
            }
            
            case DIV:
            {   
                if (LNODE != nullptr)
                {
                    if ((LNODE->type == NUMBER) && (Is_equal(LNODE->value, 0)))
                    {
                        Re_linking_subtree(tree, current_node, LNODE, RNODE);
                        return;
                    }
                }

                if (RNODE != nullptr)
                {
                    if ((RNODE->type == NUMBER) && (Is_equal(RNODE->value, 1)))
                    {
                        Re_linking_subtree(tree, current_node, LNODE, RNODE);
                        return;
                    }
                }

                break;
            }

            case POW:
            {
                if (LNODE != nullptr)
                {
                    if ((LNODE->type == NUMBER) && ((Is_equal(LNODE->value, 0)) || (Is_equal(LNODE->value, 1))))
                    {
                        Re_linking_subtree(tree, current_node, LNODE, RNODE);
                        return;
                    }
                }

                if (RNODE != nullptr)
                {
                    if ((RNODE->type == NUMBER) && ((Is_equal(RNODE->value, 0)) || (Is_equal(RNODE->value, 1))))
                    {
                        struct Node* result = RNODE;
                        Re_linking_subtree(tree, current_node, LNODE, RNODE);
                        
                        result->value = 1;

                        return;
                    }
                }

                break;
            }

            default:
                tree->error = OPERATOR_SYNTAX_ERROR;
                TREE_ASSERT_OK(tree);
                break;
        }
    }

    else if (TYPE == FUNC)
    {
        switch (OP)
        {
            case LN:
            case ARCCOS:
            {
                if ((FUNC_ARG_TYPE == NUMBER) && (Is_equal(RNODE->value, 1)))
                {
                    Replace_func(tree, current_node, 0);
                    return;

                    break;
                }
            }

            case SIN:
            case TG:
            case ARCSIN:
            case ARCTG:
            case SH:
            case TH:
            case SQRT:
            {
                if ((FUNC_ARG_TYPE == NUMBER) && (Is_equal(RNODE->value, 0)))
                {
                    Replace_func(tree, current_node, 0);
                    return;
                }

                break;
            }
            
            case COS:
            case CH:
            case EXP:
            {
                if ((FUNC_ARG_TYPE == NUMBER) && (Is_equal(RNODE->value, 0)))
                {
                    Replace_func(tree, current_node, 1);
                    return;
                }

                break;
            }

            default: break;
        }
    }
}

void Replace_func(struct Tree* tree, struct Node* current_node, double value)
{
    Subtree_destruct(RNODE);
    free(current_node->str);

    Node_fill(current_node,
              nullptr,
              NUMBER, nullptr, value, current_node->prev,
              nullptr);

    tree->size = Size_subtree(tree, tree->root);
}

void Re_linking_subtree(struct Tree* tree, struct Node* current_node, struct Node* subtree_linking, struct Node* subtree_delete)
{
    Tree_null_check(tree);

    if (current_node == tree->root) 
    {
        tree->root = subtree_linking;
        subtree_linking->prev = nullptr;
    }

    else
    {
        subtree_linking->prev = PREV;

        if (PREV->left  == current_node) PREV->left  = subtree_linking;
        if (PREV->right == current_node) PREV->right = subtree_linking;
    }
    
    tree->size = Size_subtree(tree, tree->root);

    Subtree_destruct(subtree_delete);
    Node_destruct(current_node);
}

//=========================================================================================

unsigned int Hash(const char* cmd)
{
    assert(cmd != nullptr);

    unsigned int hash = 0;
    
    for (size_t i = 0; i < strlen(cmd); ++i)
    {
        hash += (unsigned char)(*(cmd + i));
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

size_t Skip_spaces(char** string)
{   
    assert(string != nullptr);

    size_t count = 0;

    while (isspace(**string))
    {
        ++(*string);
        ++count;
    }

    return count; 
}

int Code_func(char* func)
{
    assert(func != nullptr);

    unsigned int hash_func = Hash(func);

    #define DEF_OP(operator, OP, number, code) 
    #define DEF_FUNC(func, FUNC, hash, number, code) if (hash_func == hash) return FUNC;  
    
    #include "commands.h"

    #undef DEF_OP
    #undef DEF_FUNC

    return -1;
}

char* Get_name_equation(const char* name_file)
{
    assert(name_file != nullptr);

    char* name_equation   = strdup(name_file);
    char* pointer_format  = strchr(name_equation, '.');
    if   (pointer_format != nullptr) *pointer_format = '\0';

    return name_equation;
}

int Subtree_is_number(struct Tree* tree, struct Node* current_node)
{
    Tree_null_check(tree);

    if (current_node == nullptr) return 0;

    if (TYPE == VAR)         return 0;
    if (TYPE == NUMBER)      return 1;

    #ifdef ACCURATE_CALCULATION
    if (TYPE == FUNC)        return 0;
    #else
    if (TYPE == FUNC)        return Subtree_is_number(tree, RNODE);
    #endif

    if (TYPE == OPERATION)   return Subtree_is_number(tree, LNODE) * 
                                    Subtree_is_number(tree, RNODE);

    return 0;   
}

size_t Size_subtree(struct Tree* tree, struct Node* current_node)
{
    Tree_null_check(tree);
    assert(current_node != nullptr);

    size_t count = 0;

    Node_count(tree, current_node, &count);

    return count;
}

void Node_count(struct Tree* tree, struct Node* current_node, size_t* count)
{   
    Tree_null_check(tree);
    assert(current_node != nullptr);
    assert(count        != nullptr);

    ++(*count);

    if (LNODE != nullptr) Node_count(tree, LNODE, count);
    if (RNODE != nullptr) Node_count(tree, RNODE, count);
}

struct Variable* Find_var(struct Stack* vars, unsigned int hash)
{
    assert(vars != nullptr);

    for (size_t num_var = 0; num_var < vars->size; ++num_var)
    {
        if (((struct Variable*)((vars->data)[num_var]))->hash == hash) 
            return (struct Variable*)(vars->data)[num_var];
    }
    
    return nullptr;
}

int Is_equal(double value, double number)
{
    return fabs(value - number) <= ACCURACY;
}

//======================================================================================

void Tree_text_print(struct Tree* tree)
{
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);

    char name_output[MAX_SIZE_COMMAND] = {};
    sprintf(name_output, "%s_out.txt", tree->name_equation);

    FILE* file = fopen(name_output, "w");
    
    Node_text_print(tree, tree->root, file);

    fclose(file);

    char command[MAX_SIZE_COMMAND] = {};

    sprintf(command, "iconv --from-code=UTF-8 --to-code=WINDOWS-1251 %s_out.txt -o %s_out.txt", tree->name_equation, tree->name_equation);
    system(command);
}

void Node_text_print(struct Tree* tree, struct Node* current_node, FILE* file)
{
    Tree_null_check(tree);
    assert(file != nullptr);

    if (current_node == nullptr) return;

    switch (TYPE)
    {
        case VAR:    fprintf(file, "%s", current_node->str);    break;
        case NUMBER: fprintf(file, "%lg", current_node->value); break;
        case FUNC:
        {
            switch ((int)(current_node->value))
            {
                case LN:     FUNC_PRINT("ln",     "(", ")"); break;
                case SIN:    FUNC_PRINT("sin",    "(", ")"); break;
                case COS:    FUNC_PRINT("cos",    "(", ")"); break;
                case TG:     FUNC_PRINT("tg",     "(", ")"); break;
                case CTG:    FUNC_PRINT("ctg",    "(", ")"); break;
                case ARCSIN: FUNC_PRINT("arcsin", "(", ")"); break;
                case ARCCOS: FUNC_PRINT("arccos", "(", ")"); break;
                case ARCTG:  FUNC_PRINT("arctg",  "(", ")"); break;
                case ARCCTG: FUNC_PRINT("arcctg", "(", ")"); break;
                case SH:     FUNC_PRINT("sh",     "(", ")"); break;
                case CH:     FUNC_PRINT("ch",     "(", ")"); break;
                case TH:     FUNC_PRINT("th",     "(", ")"); break;
                case CTH:    FUNC_PRINT("cth",    "(", ")"); break;
                case SQRT:   FUNC_PRINT("sqrt",   "(", ")"); break;
                case EXP:    FUNC_PRINT("e^",     "(", ")"); break;

                default:
                    tree->error = FUNC_SYNTAX_ERROR;
                    TREE_ASSERT_OK(tree);          
                    break;
            }

            break;
        }
        
        case OPERATION:
        {
            if ((current_node == tree->root) || ((PREV_TYPE(current_node) == OPERATION)     && (PREV_OP(current_node) == POW))   ||
                ((PREV_TYPE(current_node) == FUNC)      && ((PREV_OP(current_node) == SQRT) || (PREV_OP(current_node) == EXP)))  ||
                ((PREV_TYPE(current_node) == OPERATION) && ((PREV_OP(current_node) == ADD)  || (PREV_OP(current_node) == SUB))   &&
                 (TYPE == OPERATION)      && ((OP == ADD) || (OP == SUB))))
            {
                switch ((int)(current_node->value))
                {
                    case ADD: OP_PRINT(" + ");  break;
                    case SUB: OP_PRINT(" - ");  break;
                    case MUL: OP_PRINT(" * ");  break;
                    case DIV: OP_PRINT(" \\ "); break;
                    case POW: if ((LNODE->type == OPERATION) || (LNODE->type == FUNC) || 
                                 ((LNODE->type == VAR) && (strlen_n(LNODE->str) > 1)))
                            {
                                fprintf(file, "(");
                                OP_PRINT(")^(");
                                fprintf(file, ")");
                            }

                            else
                            {
                                OP_PRINT("^(");
                                fprintf(file,  ")");
                            }
                            
                            break;

                    default:
                        tree->error = OPERATOR_SYNTAX_ERROR;
                        TREE_ASSERT_OK(tree);
                        break;
                }
            }

            else
            {
                switch ((int)(current_node->value))
                {
                    case ADD: OP_BPRINT("(",    " + ",  ")"); break;
                    case SUB: OP_BPRINT("(",    " - ",  ")"); break;
                    case MUL: OP_PRINT(" * ");  break;
                    case DIV: OP_PRINT(" \\ "); break;
                    case POW: if ((LNODE->type == OPERATION) || (LNODE->type == FUNC) || 
                                 ((LNODE->type == VAR) && (strlen_n(LNODE->str) > 1)))
                            {
                                OP_BPRINT("(",  ")^(", ")");
                            }

                            else
                            {
                                OP_PRINT("^(");
                                fprintf(file, ")");
                            }
                            
                            break;


                    default:
                        tree->error = OPERATOR_SYNTAX_ERROR;
                        TREE_ASSERT_OK(tree);
                        break;
                }
            }

            break;
        }
        
        default:
            tree->error = TYPE_SYNTAX_ERROR;
            TREE_ASSERT_OK(tree);
            break;
    }
}

void Equation_tex_dump(struct Tree* tree, struct Text* text)
{
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);

    FILE* tex = Equation_tex_dump_open(tree, text);
    Equation_tex_print(tree, tree->root, tex, text);
    Equation_tex_dump_close(tree, tex);
}

FILE* Equation_tex_dump_open(struct Tree* tree, struct Text* text)
{
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);

    char name_texfile[MAX_SIZE_COMMAND] = {};
    sprintf(name_texfile, "images/%s.tex", tree->name_equation);

    FILE* file      = fopen(name_texfile, "w");

    FILE* file_text = fopen("Key_sentences.txt", "r");
    Create_text(file_text, text);
    fclose(file_text);    

    fprintf(file, "\\input{images/preambule_article}\n"
                  "\\usepackage{upgreek}\n\n\n"
                  "\\begin{document}\n\n"
                  "\\input{images/title}\n\n");

    return file;
}

void Equation_tex_print(struct Tree* tree, struct Node* begin, FILE* file, struct Text* text)
{
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);
    assert(file != nullptr);

    PRINT_RANDOM_SENTENSE();

    fprintf(file, "\n\n\\begin{dmath}\n");
    Node_tex_print(tree, begin, begin, file);
    fprintf(file, "\n\\end{dmath}\n\n");
}

void Equation_tex_print_derivate(struct Tree* tree, struct Node* before, struct Node* after, char* var, FILE* file, struct Text* text)
{
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);
    assert(file != nullptr);

    PRINT_RANDOM_SENTENSE();

    fprintf(file, "\n\n\\begin{dmath}\n"
                  " \\cfrac{d}{d %s} \\left( ", var);              
    Subtree_tex_print(tree, before, file);

    fprintf(file, " \\right) = ");

    Subtree_tex_print(tree, after, file);
    fprintf(file, " \n\\end{dmath}\n\n");
}

void Equation_tex_print_total_differential(struct Function* function, int mode, FILE* file, struct Text* text)
{   
    assert(function != nullptr);
    assert(file != nullptr);
    Tree_null_check(function->tree);
    TREE_ASSERT_OK(function->tree);
    
    PRINT_RANDOM_SENTENSE();

    fprintf(file, "\n\n\\begin{dmath}\n"
                  "d f \\left( ");

    if (mode == PRINT_VAR)
    {
        for (size_t index = 0; index < function->vars->size - 1; ++index)
        {
            fprintf(file, " %s, ", ((struct Variable*)((function->vars->data)[index]))->name);
        }

        if (function->vars->size > 0) fprintf(file, " %s", ((struct Variable*)((function->vars->data)[function->vars->size - 1]))->name);
    }

    else if (mode == PRINT_NUMBER)
    {
            for (size_t index = 0; index < function->vars->size - 1; ++index)
        {
            fprintf(file, " %lg, ", ((struct Variable*)((function->vars->data)[index]))->value);
        }

        if (function->vars->size > 0) fprintf(file, " %lg", ((struct Variable*)((function->vars->data)[function->vars->size - 1]))->value);
    }

    fprintf(file, " \\right) = ");

    if (function->derivates->size > 0)
    {
        if ((((struct Tree*)((function->derivates->data)[0]))->root->type == OPERATION && 
            (((struct Tree*)((function->derivates->data)[0]))->root->value == ADD      || 
             ((struct Tree*)((function->derivates->data)[0]))->root->value == SUB))    || 
            (((struct Tree*)((function->derivates->data)[0]))->size == 1)              && 
            (((struct Tree*)((function->derivates->data)[0]))->root->value < 0))
        {
            fprintf(file, " \\left( ");
            Tree_tex_print((struct Tree*)((function->derivates->data)[0]), file);
            fprintf(file, " \\right) ");
        }
        else Tree_tex_print((struct Tree*)((function->derivates->data)[0]), file);
        
        fprintf(file, " \\cdot d %s ", ((struct Variable*)((function->vars->data)[0]))->name);

        for (size_t index = 1; index < function->derivates->size; ++index)
        {
            fprintf(file, " + ");
            
            if ((((struct Tree*)((function->derivates->data)[index]))->root->type  == OPERATION && 
                (((struct Tree*)((function->derivates->data)[index]))->root->value == ADD       || 
                 ((struct Tree*)((function->derivates->data)[index]))->root->value == SUB))     ||
                (((struct Tree*)((function->derivates->data)[index]))->size == 1)               && 
                (((struct Tree*)((function->derivates->data)[index]))->root->value < 0))
            {
                fprintf(file, " \\left( ");
                Tree_tex_print((struct Tree*)((function->derivates->data)[index]), file);
                fprintf(file, " \\right) ");
            }
            else Tree_tex_print((struct Tree*)((function->derivates->data)[index]), file);

            fprintf(file, " \\cdot d %s ", ((struct Variable*)((function->vars->data)[index]))->name);
        }
    }

    else
    {
        fprintf(file, " 0 ");
    }

    fprintf(file, "\n\\end{dmath}\n\n");
}

void Tree_tex_print(struct Tree* tree, FILE* file)
{
    Tree_null_check(tree);
    assert(file != nullptr);

    Node_tex_print(tree, tree->root, tree->root, file);
}

void Subtree_tex_print(struct Tree* tree, struct Node* begin, FILE* file)
{
    Tree_null_check(tree);
    assert(begin != nullptr);
    assert(file  != nullptr);

    Node_tex_print(tree, begin, begin, file);
}

void Node_tex_print(struct Tree* tree, struct Node* begin, struct Node* current_node, FILE* file)
{   
    Tree_null_check(tree);
    assert(file != nullptr);

    if (current_node == nullptr) return;

    switch (TYPE)
    {
        case VAR: fprintf(file, "%s", current_node->str); break;
        case NUMBER: 
        {
            if ((current_node->value < 0) && !(current_node == begin) && 
                                             !((PREV_TYPE(current_node) == OPERATION) && (PREV_OP(current_node) == DIV)))
            {
                fprintf(file, "(%lg)", current_node->value);
            }

            else 
            {
                fprintf(file, "%lg", current_node->value);
            }

            break;
        }
        
        case FUNC:
        {
            switch ((char)(current_node->value))
            {
                case LN:     FUNC_TEXPRINT("\\ln ",     "\\left(", "\\right)"); break;
                case SIN:    FUNC_TEXPRINT("\\sin ",    "\\left(", "\\right)"); break;
                case COS:    FUNC_TEXPRINT("\\cos ",    "\\left(", "\\right)"); break;
                case TG:     FUNC_TEXPRINT("\\tg ",     "\\left(", "\\right)"); break;
                case CTG:    FUNC_TEXPRINT("\\ctg ",    "\\left(", "\\right)"); break;
                case ARCSIN: FUNC_TEXPRINT("\\arcsin ", "\\left(", "\\right)"); break;
                case ARCCOS: FUNC_TEXPRINT("\\arccos ", "\\left(", "\\right)"); break;
                case ARCTG:  FUNC_TEXPRINT("\\arctg ",  "\\left(", "\\right)"); break;
                case ARCCTG: FUNC_TEXPRINT("\\arcctg ", "\\left(", "\\right)"); break;
                case SH:     FUNC_TEXPRINT("\\sh ",     "\\left(", "\\right)"); break;
                case CH:     FUNC_TEXPRINT("\\ch ",     "\\left(", "\\right)"); break;
                case TH:     FUNC_TEXPRINT("\\th ",     "\\left(", "\\right)"); break;
                case CTH:    FUNC_TEXPRINT("\\cth ",    "\\left(", "\\right)"); break;
                case SQRT:   FUNC_TEXPRINT("\\sqrt{ ",  " ",              "}"); break;
                case EXP:    FUNC_TEXPRINT("e^{ ",      " ",              "}"); break;

                default:
                    tree->error = FUNC_SYNTAX_ERROR;
                    TREE_ASSERT_OK(tree);          
                    break;
            }

            break;
        }

        case OPERATION:
        {
            if ((current_node == begin)   || ((PREV_TYPE(current_node) == OPERATION) && (PREV_OP(current_node) == POW))          ||
                ((PREV_TYPE(current_node) == FUNC)      && ((PREV_OP(current_node) == SQRT) || (PREV_OP(current_node) == EXP)))  ||
                ((PREV_TYPE(current_node) == OPERATION) && ( PREV_OP(current_node) == DIV)) ||
                ((PREV_TYPE(current_node) == OPERATION) && ((PREV_OP(current_node) == ADD)  || (PREV_OP(current_node) == SUB))   &&
                 (TYPE == OPERATION)      && ((OP == ADD) || (OP == SUB))))
            {
                switch ((int)(current_node->value))
                {
                    case ADD: OP_TEXPRINT(" + ");                  break;
                    case SUB: OP_TEXPRINT(" - ");                  break;
                    case MUL: OP_TEXPRINT(" \\cdot ");             break;
                    case DIV: OP_BTEXPRINT("\\cfrac{", "}{", "}"); break;
                    case POW: if ((LNODE->type == OPERATION) || (LNODE->type == FUNC) || 
                                 ((LNODE->type == VAR) && (strlen_n(LNODE->str) > 1)))
                            {
                                OP_BTEXPRINT("\\left(",  "\\right)^{", "}");
                            }

                            else
                            {
                                OP_TEXPRINT("^{");
                                fprintf(file, "}");
                            }
                            
                            break;
                            
                    default:
                        tree->error = OPERATOR_SYNTAX_ERROR;
                        TREE_ASSERT_OK(tree);
                        break;
                }
            }

            else
            {
                switch ((int)(current_node->value))
                {
                    case ADD: OP_BTEXPRINT("\\left(",  " + ",  "\\right)"); break;
                    case SUB: OP_BTEXPRINT("\\left(",  " - ",  "\\right)"); break;
                    case MUL: OP_TEXPRINT(" \\cdot "                     ); break;
                    case DIV: OP_BTEXPRINT("\\cfrac{", "}{",          "}"); break;
                    case POW: 
                        {
                            if ((LNODE->type == OPERATION) || (LNODE->type == FUNC) || 
                               ((LNODE->type == VAR) && (strlen_n(LNODE->str) > 1)))
                            {
                                OP_BTEXPRINT("\\left(",  "\\right)^{", "}");
                            }

                            else
                            {
                                OP_TEXPRINT("^{");
                                fprintf(file, "}");
                            }
                            
                            break;
                        }

                    default:
                        tree->error = OPERATOR_SYNTAX_ERROR;
                        TREE_ASSERT_OK(tree);
                        break;
                }
            }

            break;
        }

        default:
            tree->error = TYPE_SYNTAX_ERROR;
            TREE_ASSERT_OK(tree);
            break;
    }
}

void Equation_tex_dump_close(struct Tree* tree, FILE* file)
{
    Tree_null_check(tree);
    assert(file != nullptr);

    char name_file[MAX_SIZE_STR] = {};
    sprintf(name_file, "images/%s", tree->name_equation);

    fprintf(file, "\\input{images/end}\n"
                  "\\end{document}\n");

    fclose(file);

    char command[MAX_SIZE_COMMAND] = {};

    sprintf(command, "pdflatex -output-directory images/ %s.tex", name_file);
    system(command);

    sprintf(command, "okular %s.pdf", name_file);
    system(command);
}