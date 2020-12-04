DEF_OP(+, ADD, 43, 
    {
        LNODE = dL; 
        RNODE = dR;
    })

DEF_OP(-, SUB, 45, 
    {
        LNODE = dL; 
        RNODE = dR;
    })

DEF_OP(*, MUL, 42, 
    {
        struct Node* l_subtree = Node_clone(LNODE, nullptr);
        struct Node* r_subtree = Node_clone(RNODE, nullptr);
        struct Node* ld_subtree = dL;
        struct Node* rd_subtree = dR;

        struct Node* left  = CNODE_OP(ld_subtree, MUL, r_subtree);
        struct Node* right = CNODE_OP(l_subtree,  MUL, rd_subtree);

        Node_fill(current_node, 
                  left,
                  OPERATION, nullptr, ADD, current_node->prev,
                  right);
                
    })

DEF_OP(/, DIV, 47, 
    {
        struct Node* l_subtree   = Node_clone(LNODE, nullptr);
        struct Node* r_subtree   = Node_clone(RNODE, nullptr);
        struct Node* denominator = Node_clone(RNODE, nullptr);
        struct Node* ld_subtree  = dL;
        struct Node* rd_subtree  = dR;

        struct Node* left  = CNODE_OP(ld_subtree, MUL, r_subtree); 
        struct Node* right = CNODE_OP(l_subtree,  MUL, rd_subtree); 

        struct Node* deg   = CNODE_NUM(2);

        struct Node* sub   = CNODE_OP(left, SUB, right);
        struct Node* pow   = CNODE_OP(denominator, POW, deg);
        
        Node_fill(current_node,
                  sub,
                  OPERATION, nullptr, DIV, current_node->prev,
                  pow);
                
    })

DEF_OP(^, POW, 94, 
    {
        int left_subtree_is_num  = Subtree_is_number(tree, current_node->left);
        int right_subtree_is_num = Subtree_is_number(tree, current_node->right);

        if (left_subtree_is_num && right_subtree_is_num)
        {
            Node_destruct(current_node->left);
            Node_destruct(current_node->right);

            Node_fill(current_node, 
                      nullptr,
                      NUMBER, nullptr, 0, current_node->prev,
                      nullptr);
        }

        else if (left_subtree_is_num && !right_subtree_is_num)
        {
            struct Node* base = LNODE;
            struct Node* deg  = RNODE;
            struct Node* num  = Node_clone(LNODE, nullptr);

            char* name_func = (char*) calloc(MAX_SIZE_NAME_FUNC, sizeof(char));
            sprintf(name_func, "ln");

            struct Node* pow  = CNODE_OP(base, POW, deg);
            struct Node* func = CNODE_FUNC(name_func, LN, num);
            
            Node_fill(current_node,
                      pow,
                      OPERATION, nullptr, MUL, current_node->prev,
                      func);
        }

        else if (!left_subtree_is_num && right_subtree_is_num)
        {
            if (Is_equal(RCALC, 1))
            {   
                Node_destruct(LNODE);
                Node_destruct(RNODE);

                Node_fill(current_node,
                          nullptr,
                          NUMBER, nullptr, 1, current_node->prev,
                          nullptr);
            }

            else
            {
                struct Node* base  = LNODE;
                struct Node* coef  = RNODE;
                struct Node* l_deg = Node_clone(RNODE, nullptr);

                struct Node* num   = CNODE_NUM(1);
                struct Node* deg   = CNODE_OP(l_deg, SUB, num);
                struct Node* pow   = CNODE_OP(base, POW, deg);

                Node_fill(current_node,
                          pow,
                          OPERATION, nullptr, MUL, current_node->prev,
                          coef);
            }
        }

        else if (!left_subtree_is_num && !right_subtree_is_num) // f^g = f^g*(dg*ln(f) + g/f*df)
        {
            struct Node* f1 = Node_clone(LNODE, nullptr);
            struct Node* f2 = Node_clone(LNODE, nullptr);
            struct Node* f3 = Node_clone(LNODE, nullptr);

            struct Node* g1 = Node_clone(LNODE, nullptr);
            struct Node* g2 = Node_clone(LNODE, nullptr);

            struct Node* df = dL;
            struct Node* dg = dR;

            char* name_func = (char*) calloc(MAX_SIZE_NAME_FUNC, sizeof(char));
            sprintf(name_func, "ln");

            struct Node* ln    = CNODE_FUNC(name_func, LN, f3);

            struct Node* l_mul = CNODE_OP(dg,    MUL,   ln);
            struct Node* div   = CNODE_OP(g2,    DIV,   f2);
            struct Node* r_mul = CNODE_OP(div,   MUL,   df);
            struct Node* add   = CNODE_OP(l_mul, ADD,   r_mul);
            struct Node* pow   = CNODE_OP(f1,    POW,   g1);
            
            Node_fill(current_node,
                      pow, 
                      OPERATION, nullptr, MUL, current_node->prev,
                      add);

        }        
    })

DEF_FUNC(ln,     LN,     0x101ba648, 65, 
    {
        struct Node* right = Node_clone(RNODE, nullptr);
        struct Node* left  = dR;
                    
        free(FUNCTION);

        Node_fill(current_node,
                  left, 
                  OPERATION, nullptr, DIV, current_node->prev, 
                  right);  

    })

DEF_FUNC(sin,    SIN,    0xbf987f58, 66, 
    {
        struct Node* arg   = Node_clone(RNODE, nullptr);
        struct Node* right = dR;

        char* name_func = (char*) calloc(MAX_SIZE_NAME_FUNC, sizeof(char));
        sprintf(name_func, "cos");

        struct Node* func = CNODE_FUNC(name_func, COS, arg);

        free(FUNCTION);

        Node_fill(current_node,
                  func,
                  OPERATION, nullptr, MUL, current_node->prev,
                  right); 

    })

DEF_FUNC(cos,    COS,    0x238fe9,   67, 
    {
        struct Node* arg   = Node_clone(RNODE, nullptr);
        struct Node* right = dR;

        char* name_func = (char*) calloc(MAX_SIZE_NAME_FUNC, sizeof(char));
        sprintf(name_func, "sin");

        struct Node* num  = CNODE_NUM(-1);
        struct Node* func = CNODE_FUNC(name_func, SIN, arg);
        struct Node* mul  = CNODE_OP(num, MUL, func);

        free(FUNCTION);

        Node_fill(current_node,
                  mul,
                  OPERATION, nullptr, MUL, current_node->prev,
                  right);

    })

DEF_FUNC(tg,     TG,     0xf5a17ed,  68, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* left = dR;

        char* name_func = (char*) calloc(MAX_SIZE_NAME_FUNC, sizeof(char));
        sprintf(name_func, "cos");

        struct Node* deg  = CNODE_NUM(2);
        struct Node* func = CNODE_FUNC(name_func, COS, arg);
        struct Node* pow  = CNODE_OP(func, POW, deg);

        free(FUNCTION);

        Node_fill(current_node,
                  left,
                  OPERATION, nullptr, DIV, current_node->prev,
                  pow);

    })

DEF_FUNC(ctg,    CTG,    0xdf88c1c4, 69, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* left = dR;

        char* name_func = (char*) calloc(MAX_SIZE_STR, sizeof(char));
        sprintf(name_func, "sin");

        struct Node* deg  = CNODE_NUM(2);
        struct Node* func = CNODE_FUNC(name_func, SIN, arg);
        struct Node* pow  = CNODE_OP(func, POW, deg);
        struct Node* div  = CNODE_OP(left, DIV, pow);
        struct Node* num  = CNODE_NUM(-1);

        free(FUNCTION);

        Node_fill(current_node,
                  num,
                  OPERATION, nullptr, MUL, current_node->prev,
                  div);

    })

DEF_FUNC(arcsin, ARCSIN, 0xbf69dd95, 70, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* left = dR;

        char* name_func = (char*) calloc(MAX_SIZE_NAME_FUNC, sizeof(char));
        sprintf(name_func, "sqrt");

        struct Node* num1 = CNODE_NUM(1);
        struct Node* num2 = CNODE_NUM(2);

        struct Node* pow  = CNODE_OP(arg,  POW, num2);
        struct Node* sub  = CNODE_OP(num1, SUB, pow);
        struct Node* func = CNODE_FUNC(name_func, SQRT, sub);
        
        free(FUNCTION);

        Node_fill(current_node,
                  left,
                  OPERATION, nullptr, DIV, current_node->prev,
                  func);

    })

DEF_FUNC(arccos, ARCCOS, 0x9a380fc2, 71, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* left = dR;

        char* name_func = (char*) calloc(MAX_SIZE_NAME_FUNC, sizeof(char));
        sprintf(name_func, "sqrt");
        
        struct Node* num  = CNODE_NUM(-1);
        struct Node* num1 = CNODE_NUM(1);
        struct Node* num2 = CNODE_NUM(2);

        struct Node* pow  = CNODE_OP(arg,  POW, num2);
        struct Node* sub  = CNODE_OP(num1, SUB, pow);
        struct Node* func = CNODE_FUNC(name_func, SQRT, sub);
        struct Node* div  = CNODE_OP(left, DIV, func);

        free(FUNCTION);

        Node_fill(current_node,
                  num,
                  OPERATION, nullptr, MUL, current_node->prev,
                  div);

    })

DEF_FUNC(arctg,  ARCTG,  0x3cffbcdb, 72, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* left = dR;

        struct Node* num1 = CNODE_NUM(1);
        struct Node* num2 = CNODE_NUM(2);

        struct Node* pow  = CNODE_OP(arg,  POW, num2);
        struct Node* add  = CNODE_OP(num1, ADD, pow);

        free(FUNCTION);

        Node_fill(current_node,
                  left,
                  OPERATION, nullptr, DIV, current_node->prev,
                  add);

    })

DEF_FUNC(arcctg, ARCCTG, 0xbc7147b0, 73, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* left = dR;

        struct Node* num  = CNODE_NUM(-1);
        struct Node* num1 = CNODE_NUM(1);
        struct Node* num2 = CNODE_NUM(2);

        struct Node* pow  = CNODE_OP(arg,  POW, num2);
        struct Node* add  = CNODE_OP(num1, ADD, pow);
        struct Node* div  = CNODE_OP(left, DIV, add);

        free(FUNCTION);

        Node_fill(current_node,
                    num,
                    OPERATION, nullptr, MUL, current_node->prev,
                    div);
    })

DEF_FUNC(sh,     SH,     0x6c8057cc, 74, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* right = dR;

        char* name_func = (char*) calloc(MAX_SIZE_NAME_FUNC, sizeof(char));
        sprintf(name_func, "ch");

        struct Node* func = CNODE_FUNC(name_func, CH, arg);

        free(FUNCTION);

        Node_fill(current_node,
                  func,
                  OPERATION, nullptr, MUL, current_node->prev,
                  right);

    })

DEF_FUNC(ch,     CH,     0x4ab1a5ab, 75, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* right = dR;

        char* name_func = (char*) calloc(MAX_SIZE_NAME_FUNC, sizeof(char));
        sprintf(name_func, "sh");

        struct Node* func = CNODE_FUNC(name_func, SH, arg);

        free(FUNCTION);

        Node_fill(current_node,
                  func,
                  OPERATION, nullptr, MUL, current_node->prev,
                  right);

    })

DEF_FUNC(th,     TH,     0x6a4bcdcf, 76, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* left = dR;

        char* name_func = (char*) calloc(MAX_SIZE_NAME_FUNC, sizeof(char));
        sprintf(name_func, "ch");

        struct Node* deg  = CNODE_NUM(2);
        struct Node* func = CNODE_FUNC(name_func, CH, arg);
        struct Node* pow  = CNODE_OP(func, POW, deg);

        free(FUNCTION);

        Node_fill(current_node,
                  left,
                  OPERATION, nullptr, DIV, current_node->prev,
                  pow);

    })

DEF_FUNC(cth,    CTH,    0xf562ed78, 77, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* left = dR;

        char* name_func = (char*) calloc(MAX_SIZE_STR, sizeof(char));
        sprintf(name_func, "sh");

        struct Node* deg  = CNODE_NUM(2);
        struct Node* func = CNODE_FUNC(name_func, SH, arg);
        struct Node* pow  = CNODE_OP(func, POW, deg);
        struct Node* div  = CNODE_OP(left, DIV, pow);
        struct Node* num  = CNODE_NUM(-1);

        free(FUNCTION);

        Node_fill(current_node,
                  num,
                  OPERATION, nullptr, MUL, current_node->prev,
                  div);
    })

DEF_FUNC(sqrt,   SQRT,   0x145c7701, 78, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* left = dR;

        char* name_func = (char*) calloc(MAX_SIZE_STR, sizeof(char));
        sprintf(name_func, "sqrt");

        struct Node* num  = CNODE_NUM(2);
        struct Node* func = CNODE_FUNC(name_func, SQRT, arg);
        struct Node* mul  = CNODE_OP(num, MUL, func);

        free(FUNCTION);

        Node_fill(current_node,
                  left,
                  OPERATION, nullptr, DIV, current_node->prev,
                  mul);

    })

DEF_FUNC(exp,    EXP,    0xe2313450, 79, 
    {
        struct Node* arg  = Node_clone(RNODE, nullptr);
        struct Node* right = dR;

        char* name_func = (char*) calloc(MAX_SIZE_STR, sizeof(char));
        sprintf(name_func, "exp");

        struct Node* func = CNODE_FUNC(name_func, EXP, arg);

        free(FUNCTION);

        Node_fill(current_node,
                  func, 
                  OPERATION, nullptr, MUL, current_node->prev,
                  right);
                  
    })
