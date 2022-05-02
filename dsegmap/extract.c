#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int gethexc(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else
        return c - 'A' + 10;
}

int gethex(char *sz)
{
    return gethexc(sz[0]) * 4096 + gethexc(sz[1]) * 256 + gethexc(sz[2]) * 16 + gethexc(sz[3]);
}

int main()
{
    int base = 0;
    int spcount = 0;
    char sz[1024];
    while (gets(sz))
    {
        if (sz[17] == '_')
        {
            if (strcmp(&sz[17], "_s_player") == 0)
            {
                spcount++;
                if (spcount == 2)
                {
                    base = gethex(&sz[6]);
                    printf("%04X %s\n", base, &sz[17]);
                }
            }
            else if (spcount == 2)
            {
                if (sz[18] == 's' && sz[19] == '_')
                    printf("%04X %s\n", gethex(&sz[6]) - base, &sz[17]);
            }
            else if (strcmp(&sz[17], "_statobjlist") == 0
                     || strcmp(&sz[17], "_objlist") == 0
                     || strcmp(&sz[17], "_spotvis") == 0)
            {
                printf("%04X %s\n", gethex(&sz[6]), &sz[17]);
            }
        }
    }

    return 0;
}
