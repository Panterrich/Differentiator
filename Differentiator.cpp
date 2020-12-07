#include "Differentiator.h"
#include "DSL.h"

void Tree_create(struct Tree* tree, const char* name_equation)
{
    Tree_null_check(tree);
    assert(name_equation != nullptr);

    char command[MAX_SIZE_COMMAND] = {};
    sprintf(command, "iconv -f WINDOWS-1251 -t UTF-8 %s -o test.txt && mv test.txt %s", name_equation, name_equation);
    system(command);

    FILE* text = fopen(name_equation, "rb");

    if (text == nullptr)
    {
        tree->error = TREE_WRONG_NAME_DATA_EQUATION;
        TREE_ASSERT_OK(tree);
    }

    struct Text equation = {};
    Create_text(text, &equation);

    fclose(text);

    if (tree->root != nullptr)
    {
        Tree_destruct(tree);
    }
    
    tree->name_equation = Get_name_equation(name_equation);
    
    char* current_operation = equation.lines[0].str;
    Skip_spaces(&current_operation);

    tree->name_table = (struct Stack*) calloc(1, sizeof(struct Stack));
    STACK_CONSTUCT(tree->name_table, INITIAL_VAR);

    tree->root = Node_create(tree, nullptr, &current_operation);

    Free_memory(&equation);

    sprintf(command, "iconv --from-code=UTF-8 --to-code=WINDOWS-1251 %s -o %s", name_equation, name_equation);
    system(command);
}

struct Node* Node_create(struct Tree* tree, struct Node* previos_node, char** current_opetation)
{
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);
    assert(current_opetation != nullptr);

    struct Node* current_node = (struct Node*) calloc(1, sizeof(struct Node));
    size_t shift = 0;

    ++(tree->size);

    Skip_spaces(current_opetation);

    if (**current_opetation == '(')
    {    
        *current_opetation += 1;

        LNODE = Node_create(tree, current_node, current_opetation);

        Node_fill(current_node,
                  LNODE,
                  OPERATION, nullptr, Get_operator(current_opetation), previos_node, 
                  RNODE);

        RNODE = Node_create(tree, current_node, current_opetation);

        Skip_spaces(current_opetation);
        
        if (**current_opetation == ')') 
        {
            ++(*current_opetation);
            Skip_spaces(current_opetation);
        }
        else
        {
            tree->error = EQUATION_SYNTAX_ERROR;
            TREE_ASSERT_OK(tree);
        }
    }

    else if (sscanf(*current_opetation, " %lg%n", &current_node->value, &shift))
    {
        Node_fill(current_node,
                  nullptr,
                  NUMBER, nullptr, current_node->value, previos_node,
                  nullptr);

        *current_opetation += shift;
        Skip_spaces(current_opetation);
    }

    else if (Find_func(*current_opetation))
    {
        char* name_func = Get_name_func(current_opetation);

        Node_fill(current_node, 
                  nullptr, 
                  FUNC, name_func, Code_func(name_func), previos_node,
                  Node_create(tree, current_node, current_opetation));
        
        Skip_spaces(current_opetation);
        
        if (**current_opetation == ')') 
        {
            ++(*current_opetation);
            Skip_spaces(current_opetation);
        }
        else
        {
            tree->error = EQUATION_SYNTAX_ERROR;
            TREE_ASSERT_OK(tree);
        }
    }

    else
    {
        char* name_variable = (char*) calloc(MAX_SIZE_NAME_VARIABLES, sizeof(char));
        sscanf(*current_opetation, "%[^() ]", name_variable);

        struct Variable* variable = (struct Variable*) calloc(1, sizeof(struct Variable));

        variable->name  = name_variable;
        variable->hash  = Hash(name_variable);
        variable->value = NAN;
        
        Stack_push(tree->name_table, variable);

        Node_fill(current_node,
                  nullptr,
                  VAR, name_variable, NAN, previos_node,
                  nullptr);

        *current_opetation += strlen(name_variable);
        Skip_spaces(current_opetation);
    }

    return current_node;
}

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
                    case ADD: OP_PRINT("\0", " + ",  "\0"); break;
                    case SUB: OP_PRINT("\0", " - ",  "\0"); break;
                    case MUL: OP_PRINT("\0", " * ",  "\0"); break;
                    case DIV: OP_PRINT("\0", " \\ ", "\0"); break;
                    case POW: if ((LNODE->type == OPERATION) || (LNODE->type == FUNC) || 
                                 ((LNODE->type == VAR) && (strlen_n(LNODE->str) > 1)))
                            {
                                OP_PRINT("(",  ")^(", ")");
                            }

                            else
                            {
                                OP_PRINT("\0",  "^(", ")");
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
                    case ADD: OP_PRINT("(",    " + ",  ")"); break;
                    case SUB: OP_PRINT("(",    " - ",  ")"); break;
                    case MUL: OP_PRINT("\0",   " * ",   "\0"); break;
                    case DIV: OP_PRINT("\0",   " \\ ",  "\0"); break;
                    case POW: if ((LNODE->type == OPERATION) || (LNODE->type == FUNC) || 
                                 ((LNODE->type == VAR) && (strlen_n(LNODE->str) > 1)))
                            {
                                OP_PRINT("(",  ")^(", ")");
                            }

                            else
                            {
                                OP_PRINT("\0",  "^(", ")");
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

unsigned int Hash(const char* cmd)
{
    assert(cmd != nullptr);

    unsigned int hash = 0;
    
    for (int i = 0; i < strlen(cmd); ++i)
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

void Equation_tex_dump(struct Tree* tree, struct Text* text)
{
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);

    FILE* tex = Equation_tex_dump_open(tree, text);
    Equation_tex_print(tree, tex, text);
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

void Equation_tex_print(struct Tree* tree, FILE* file, struct Text* text)
{
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);
    assert(file != nullptr);

    PRINT_RANDOM_SENTENSE();

    BEGIN_AND_RESIZE_FORMULA();

    Node_tex_print(tree, tree->root, file);
    fprintf(file, "$\n}\n\\end{equation}\n\n");
}

void Node_tex_print(struct Tree* tree, struct Node* current_node, FILE* file)
{   
    Tree_null_check(tree);
    assert(file != nullptr);

    if (current_node == nullptr) return;

    switch (TYPE)
    {
        case VAR: fprintf(file, "%s", current_node->str); break;
        case NUMBER: 
        {
            if ((current_node->value < 0) && !(current_node == tree->root) && 
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
                case SQRT:   FUNC_TEXPRINT("\\sqrt{ ",  "\0",             "}"); break;
                case EXP:    FUNC_TEXPRINT("e^{ ",      "\0",             "}"); break;

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
                ((PREV_TYPE(current_node) == OPERATION) && ( PREV_OP(current_node) == DIV)) ||
                ((PREV_TYPE(current_node) == OPERATION) && ((PREV_OP(current_node) == ADD)  || (PREV_OP(current_node) == SUB))   &&
                 (TYPE == OPERATION)      && ((OP == ADD) || (OP == SUB))))
            {
                switch ((int)(current_node->value))
                {
                    case ADD: OP_TEXPRINT("\0",         " + ",         "\0"); break;
                    case SUB: OP_TEXPRINT("\0",         " - ",         "\0"); break;
                    case MUL: OP_TEXPRINT("\0",         " \\cdot ",    "\0"); break;
                    case DIV: OP_TEXPRINT("\\cfrac{", "}{",             "}"); break;
                    case POW: if ((LNODE->type == OPERATION) || (LNODE->type == FUNC) || 
                                 ((LNODE->type == VAR) && (strlen_n(LNODE->str) > 1)))
                            {
                                OP_TEXPRINT("\\left(",  "\\right)^{", "}");
                            }

                            else
                            {
                                OP_TEXPRINT("\0",  "^{", "}");
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
                    case ADD: OP_TEXPRINT("\\left(",  " + ",      "\\right)"); break;
                    case SUB: OP_TEXPRINT("\\left(",  " - ",      "\\right)"); break;
                    case MUL: OP_TEXPRINT("\0",       " \\cdot ",       "\0"); break;
                    case DIV: OP_TEXPRINT("\\cfrac{", "}{",              "}"); break;
                    case POW: 
                        {
                            if ((LNODE->type == OPERATION) || (LNODE->type == FUNC) || 
                               ((LNODE->type == VAR) && (strlen_n(LNODE->str) > 1)))
                            {
                                OP_TEXPRINT("\\left(",  "\\right)^{", "}");
                            }

                            else
                            {
                                OP_TEXPRINT("\0",  "^{", "}");
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

    char name_file[MAX_SIZE_COMMAND] = {};
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

char* Get_name_func(char** pointer_to_name_func)
{
    assert(pointer_to_name_func != nullptr);

    char* func_arg = strchr(*pointer_to_name_func, '(');
    *func_arg = '\0';

    char* func = strdup(*pointer_to_name_func);
    *pointer_to_name_func = func_arg + 1;

    return func;
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

int Get_operator(char** current_operator)
{
    assert(current_operator != nullptr);

    #define DEF_OP(operator, OP, number, code) if (**current_operator == number) \
                                               {                                 \
                                                   ++(*current_operator);        \
                                                   return OP;                    \
                                               }

    #define DEF_FUNC(func, FUNC, hash, number, code)
    
    #include "commands.h"

    #undef DEF_OP
    #undef DEF_FUNC

    ++(*current_operator);
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

    #ifndef ACCURATE_CALCULATION
    if (TYPE == FUNC)        return Subtree_is_number(tree, RNODE);
    #else
    if (TYPE == FUNC)        return 0;
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

int Find_func(char* current_symbol)
{
    assert(current_symbol != nullptr);

    while(!isspace(*current_symbol))
    {
        if (*current_symbol == '(') return 1;
        if (*current_symbol == ')') return 0;

        ++current_symbol;
    }
    
    return 0;
}

struct Variable* Find_var(struct Tree* tree, unsigned int hash)
{
    Tree_null_check(tree);

    for (size_t num_var = 0; num_var < tree->name_table->size; ++num_var)
    {
        if (((struct Variable*)(tree->name_table->data))[num_var].hash == hash) 
            return (struct Variable*)&(tree->name_table->data)[num_var];
    }
    
    return nullptr;
}

int Is_equal(double value, double number)
{
    return fabs(value - number) <= ACCURACY;
}

struct Node* Derivative(struct Tree* tree, struct Node* current_node, char* var, unsigned int hash_var, FILE* file, struct Text* text)
{
    Tree_null_check(tree);
    assert(current_node != nullptr);
    assert(var  != nullptr);
    assert(file != nullptr);

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

    return current_node;
}

double Evaluation(struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text)
{   
    Tree_null_check(tree);
    TREE_ASSERT_OK(tree);
    assert(current_node != nullptr);

    switch (TYPE)
    {
        case NUMBER: return current_node->value;
        case VAR:    return Find_var(tree, Hash(current_node->str))->value;
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

        default:
            tree->error = TYPE_SYNTAX_ERROR;
            TREE_ASSERT_OK(tree);
            break;
    }

    return 0;
}

void Optimization(struct Tree* tree, FILE* file, struct Text* text)
{
    size_t old_size = -1;
    
    while (tree->size != old_size)
    {   
        old_size = tree->size;

        Constant_folding(tree, tree->root, file, text);
        Neutral_delete(tree, tree->root, file, text);
        Pow_folding(tree, tree->root, file, text);
        
        Equation_tex_print(tree, file, text);
    }    
}

void Constant_folding(struct Tree* tree, struct Node* current_node, FILE* file, struct Text* text)
{
    if (!Subtree_is_number(tree, current_node))
    {
        if (LNODE != nullptr) Constant_folding(tree, LNODE, file, text);
        if (RNODE != nullptr) Constant_folding(tree, RNODE, file, text);
    }
    
    else
    {
        double value = Evaluation(tree, current_node, file, text);

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
}

void Re_linking_subtree(struct Tree* tree, struct Node* current_node, struct Node* subtree_linking, struct Node* subtree_delete)
{
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

// struct Node* Total_derivative(struct Tree* tree, struct Node* current_node, char* var, unsigned int hash_var, FILE* file, struct Text* text)
// {
//     struct Stack deff = {};
//     STACK_CONSTUCT(&deff, INITIAL_VAR);

//     for (size_t count_var = 0; count_var < tree->name_table->size; ++count_var)
//     {
//         struct Node* der_var = Node_clone(tree->root, nullptr);

//         Stack_push(&deff, Derivative(tree, der_var, ((struct Variable*)(tree->name_table->data))[count_var].name, 
//                     ((struct Variable*)(tree->name_table->data))[count_var].hash, file, text));
//     }
// }