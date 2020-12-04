#define LNODE               current_node->left
#define RNODE               current_node->right
#define FUNCTION            current_node->str
#define FUNC_ARG_TYPE       current_node->right->type
#define FUNC_ARG            current_node->right->str
#define OP                  (char)current_node->value

#define CNODE_OP(L_NODE, OPERATOR, R_NODE) Node_create_and_fill(L_NODE,                                \
                                                                OPERATION, nullptr, OPERATOR, nullptr, \
                                                                R_NODE)

#define CNODE_NUM(num) Node_create_and_fill(nullptr,                        \
                                            NUMBER, nullptr, num, nullptr,  \
                                            nullptr)

#define CNODE_FUNC(name, num, arg) Node_create_and_fill(nullptr,                  \
                                                        FUNC, name, num, nullptr, \
                                                        arg)

#define L_TEXPRINT  if (LNODE != nullptr) Node_tex_print(tree,  LNODE, file);
#define R_TEXPRINT  if (RNODE != nullptr) Node_tex_print(tree,  RNODE, file);
#define L_PRINT     if (LNODE != nullptr) Node_text_print(tree, LNODE, file);
#define R_PRINT     if (RNODE != nullptr) Node_text_print(tree, RNODE, file);

#define dL Derivative(tree, LNODE, var, hash_var, file, text)
#define dR Derivative(tree, RNODE, var, hash_var, file, text)

#define LCALC Evaluation(tree, current_node->left,  file, text)
#define RCALC Evaluation(tree, current_node->right, file, text)

#define OP_TEXPRINT(start, mid, end) fprintf(file, start); \
                                     L_TEXPRINT;           \
                                     fprintf(file, mid);   \
                                     R_TEXPRINT;           \
                                     fprintf(file, end);  

#define OP_PRINT(start, mid, end) fprintf(file, start);    \
                                  L_PRINT;                 \
                                  fprintf(file, mid);      \
                                  R_PRINT;                 \
                                  fprintf(file, end);  



#define FUNC_TEXPRINT(name, lbracket, rbracket)                                           \
    if ((FUNC_ARG_TYPE == FUNC) || (OP == EXP) || (OP == SQRT) ||                         \
       ((FUNC_ARG_TYPE == VAR)  && (strlen_n(FUNC_ARG) > 1)))                             \
    {                                                                                     \
        fprintf(file, name lbracket);                                                     \
        R_TEXPRINT;                                                                       \
        fprintf(file, rbracket);                                                          \
    }                                                                                     \
                                                                                          \
    else                                                                                  \
    {                                                                                     \
        fprintf(file, name);                                                              \
        R_TEXPRINT;                                                                       \
    }

#define FUNC_PRINT(name, lbracket, rbracket)                                              \
    if ((FUNC_ARG_TYPE == FUNC) || ((FUNC_ARG_TYPE == VAR)  && (strlen_n(FUNC_ARG) > 1))) \
    {                                                                                     \
        fprintf(file, name lbracket);                                                     \
        R_PRINT;                                                                          \
        fprintf(file, rbracket);                                                          \
    }                                                                                     \
                                                                                          \
    else                                                                                  \
    {                                                                                     \
        fprintf(file, name " ");                                                          \
        R_PRINT;                                                                          \
    }
    
