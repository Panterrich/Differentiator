#include "string.h"
#include "stdio.h"

unsigned int Hash(const char* cmd)
{
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

char foo(char c)
{
    return c;
}

int main()
{
    // #define DEF_OP(name, num, hash, argc, argv, code) printf("%s: 0x%x \n", #name, Hash(#name));
    // #include "commands.h"
    // #undef DEF_OP
}