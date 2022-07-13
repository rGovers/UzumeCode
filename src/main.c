#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UzuParser.h"
#include "UzuInterpreter.h"

#define BUFFER_SIZE 1024

int main(int a_argc, char* a_argv[])
{
    if (a_argc < 1)
    {
        printf("No Filename \n");

        return -1;
    }

    char buff[BUFFER_SIZE];
    unsigned int size = 0;
    char* str = 0;

    FILE* file = fopen(a_argv[a_argc - 1], "r");
    if (file != NULL)
    {
        while (fgets(buff, BUFFER_SIZE, file) != NULL)
        {
            if (str != NULL)
            {
                str = realloc(str, size + BUFFER_SIZE);
            }
            else
            {
                str = malloc(BUFFER_SIZE);
            }

            const int len = strlen(buff);
            memcpy(str + size, buff, len);
            size += len;
        }

        fclose(file);

        UzuParser* parser = uzucode_parser_init(str);

        UzuInterpreter* interpreter = uzucode_interpreter_init(parser);

        uzucode_parser_destroy(parser);

        uzucode_interpreter_run(interpreter);

        uzucode_interpreter_destroy(interpreter);

        return 0;
    }

    printf("Cannot open file \n");

    return -1;
}