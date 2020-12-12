#include "Differentiator.h"

int main(int argc, char* argv[])
{
    srand((unsigned int)time(nullptr));

    const char* name_equation = nullptr;

    if (argc == 1)
    {
        name_equation = "Equation.txt";
    }

    else if (argc == 2)
    {
        name_equation = argv[1];
    }

    struct Text text = {};
    struct Function function = {};

    FUNCTION_CONSTRUCT(&function);
    struct Tree* tree = function.tree;

    Function_create(&function, name_equation);

    Tree_graph(tree);

    FILE* file = Equation_tex_dump_open(tree, &text);
    Equation_tex_print(tree, tree->root, file, &text);

    Total_derivative(&function, file, &text);
    
    Equation_tex_dump_close(tree, file);

    Tree_destruct(tree);
}