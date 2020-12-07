#include "Differentiator.h"

int main(int argc, char* argv[])
{
    srand(time(nullptr));

    const char* name_equation = nullptr;

    if (argc == 1)
    {
        name_equation = "Equation.txt";
    }

    else if (argc == 2)
    {
        name_equation = argv[1];
    }

    struct Tree tree = {};
    TREE_CONSTRUCT(&tree);

    struct Text text = {};
    
    Tree_create(&tree, name_equation);

    Tree_graph(&tree);

    FILE* file = Equation_tex_dump_open(&tree, &text);

    Equation_tex_print(&tree, file, &text);

    tree.root = Derivative(&tree, tree.root, "x", Hash("x"), file, &text);

    Tree_graph(&tree);

    Optimization(&tree, file, &text);

    Tree_graph(&tree);
    
    Equation_tex_dump_close(&tree, file);


    FILE* log1 = fopen("log.txt", "a");

    Tree_dump(&tree, log1);

    fclose(log1);
}