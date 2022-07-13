#include "UzuParser.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void uzucode_parse_pushInstruction(UzuParserFunc* a_func, UzuParserInstruc* a_instruc)
{
    if (a_func->Instructions != NULL)
    {
        a_func->Instructions = realloc(a_func->Instructions, sizeof(UzuParserInstruc*) * (a_func->InstructionCount + 1));
    }
    else
    {
        a_func->Instructions = malloc(sizeof(UzuParserInstruc) * (a_func->InstructionCount + 1));
    }

    a_func->Instructions[a_func->InstructionCount] = a_instruc;
    ++a_func->InstructionCount;
}
UzuParserInstruc* uzucode_parse_pushEmpty(const char* a_name, UzuParserFunc* a_func)
{
    UzuParserInstruc* instruc = malloc(sizeof(UzuParserInstruc));

    instruc->Name = a_name;
    instruc->ReturnVar = NULL;
    instruc->VarA = NULL;
    instruc->VarB = NULL;
    instruc->ConstantA = NULL;
    instruc->ConstantB = NULL;

    uzucode_parse_pushInstruction(a_func, instruc);

    return instruc;
}

void uzucode_parse_pushJumpStack(unsigned int** a_stack, unsigned int* a_stackSize, unsigned int a_index)
{
    if (*a_stack != NULL)
    {
        *a_stack = realloc(*a_stack, sizeof(unsigned int*) * (*a_stackSize + 1));
    }
    else
    {
        *a_stack = malloc(sizeof(unsigned int*) * (*a_stackSize + 1));
    }

    *a_stack[*a_stackSize] = a_index;
    ++*a_stackSize;
}
UzuParserInstruc* uzucode_parse_getInstruction(UzuParserFunc* a_func, const char** a_str)
{
    const char* s = strchr(*a_str, ' ');
    if (s == NULL)
    {
        s = strchr(*a_str, '\n');
        if (s == NULL)
        {
            *a_str = NULL;

            return NULL;
        }
    }

    const int len = (int)(s - *a_str) + 1;
    char* n = malloc(len);
    // memcpy(n, *a_str, len);
    for (int i = 0; i < len; ++i)
    {
        n[i] = (*a_str)[i];
        if (n[i] == '\n' || n[i] == ' ')
        {
            n[i] = 0;
            
            break;
        }
    }
    n[len - 1] = 0;

    if (strcmp(n, PRINT_INSTRUC) == 0)
    {
        UzuParserInstruc* instruc = malloc(sizeof(UzuParserInstruc));
        instruc->Name = n;
        instruc->ReturnVar = NULL;
        instruc->VarA = NULL;
        instruc->VarB = NULL;
        instruc->ConstantA = NULL;
        instruc->ConstantB = NULL;

        const char* q = strchr(s, '"');
        const char* nl = strchr(s, '\n');
        assert(nl != NULL);

        if (q != NULL && q < nl)
        {
            const char* qE = strchr(q + 1, '"');
            assert(qE != NULL);

            const int len = qE - q;
            instruc->ConstantA = malloc(len);
            memcpy(instruc->ConstantA, q + 1, len);
            instruc->ConstantA[len - 1] = 0;
        }
        else
        {
            const int len = nl - s;
            instruc->VarA = malloc(len);
            for (int i = 0; i < len; ++i)
            {
                instruc->VarA[i] = s[i + 1];

                if (instruc->VarA[i] == ' ' || instruc->VarA[i] == '\n')
                {
                    instruc->VarA[i] = 0;

                    break;
                }
            }
            instruc->VarA[len - 1] = 0;
        }

        uzucode_parse_pushInstruction(a_func, instruc);

        *a_str = nl;

        return instruc;
    }
    else if (strcmp(n, FUNCSTART) == 0)
    {
        UzuParserInstruc* instruc = malloc(sizeof(UzuParserInstruc));
        instruc->Name = n;
        instruc->ReturnVar = NULL;
        instruc->VarA = NULL;
        instruc->VarB = NULL;
        instruc->ConstantA = NULL;
        instruc->ConstantB = NULL;

        const char* nl = strchr(s, '\n');
        assert(nl != NULL);

        const int len = nl - s;
        instruc->ConstantA = malloc(len);
        for (int i = 0; i < len; ++i)
        {
            instruc->ConstantA[i] = s[i + 1];

            if (instruc->ConstantA[i] == ' ')
            {
                instruc->ConstantA[i] = 0;

                break;
            }
        }
        instruc->ConstantA[len - 1] = 0;

        uzucode_parse_pushInstruction(a_func, instruc);

        *a_str = nl;

        return instruc;
    }
    else if (strcmp(n, READ_INSTRUC) == 0)
    {
        UzuParserInstruc* instruc = malloc(sizeof(UzuParserInstruc));
        instruc->Name = n;
        instruc->ReturnVar = NULL;
        instruc->VarA = NULL;
        instruc->VarB = NULL;
        instruc->ConstantA = NULL;
        instruc->ConstantB = NULL;

        const char* nl = strchr(s, '\n');
        assert(nl != NULL);

        const int len = nl - s;
        instruc->VarA = malloc(len);
        for (int i = 0; i < len; ++i)
        {
            instruc->VarA[i] = s[i + 1];

            if (instruc->VarA[i] == ' ')
            {
                instruc->VarA[i] = 0;

                break;
            }
        }
        instruc->VarA[len - 1] = 0;

        uzucode_parse_pushInstruction(a_func, instruc);

        *a_str = nl;

        return instruc;
    }
    else if(strcmp(n, LOOPSTART) == 0)
    {
        *a_str = s;

        return uzucode_parse_pushEmpty(n, a_func);
    }
    else if (strcmp(n, LOOPEND) == 0)
    {
        *a_str = s;

        return uzucode_parse_pushEmpty(n, a_func);
    }
    else if (strcmp(n, IFSTART) == 0)
    {
        const char* nx = s + 1;
        uzucode_parse_getInstruction(a_func, &nx);

        *a_str = nx;

        return uzucode_parse_pushEmpty(n, a_func);
    }
    else if (strcmp(n, IFEND) == 0)
    {
        *a_str = s;

        return uzucode_parse_pushEmpty(n, a_func);
    }
    else if (strcmp(n, BREAK_INSTRUC) == 0)
    {
        *a_str = s;

        return uzucode_parse_pushEmpty(n, a_func);
    }
    else if (strcmp(n, RETURN_INSTRUC) == 0)
    {
        UzuParserInstruc* instruc = malloc(sizeof(UzuParserInstruc));
        instruc->Name = n;
        instruc->ReturnVar = NULL;
        instruc->VarA = NULL;
        instruc->VarB = NULL;
        instruc->ConstantA = NULL;
        instruc->ConstantB = NULL;

        const char* nl = strchr(s, '\n');
        assert(nl != NULL);
        const char* q = strchr(s, '"');

        if (q != NULL && q < nl)
        {
            const char* qE = strchr(q + 1, '"');
            assert(qE != NULL);

            const int len = (int)(qE - q);
            instruc->ConstantA = malloc(len);
            memcpy(instruc->ConstantA, q + 1, len);
            instruc->ConstantA[len - 1] = 0;

            *a_str = qE + 1;
        }
        else
        {
            const int len = (int)(nl - s);
            instruc->VarA = malloc(len);
            for (int i = 0; i < len; ++i)
            {
                instruc->VarA[i] = s[i + 1];
                if (instruc->VarA[i] == ' ' || instruc->VarA[i] == '\n')
                {
                    instruc->VarA[i] = 0;

                    break;
                }
            }
            instruc->VarA[len - 1] = 0;

            *a_str = nl;
        }

        uzucode_parse_pushInstruction(a_func, instruc);

        return instruc;
    }
    else if (strcmp(n, COMPARE_INSTRUC) == 0)
    {
        *a_str = s;

        return uzucode_parse_pushEmpty(n, a_func);
    }
    else
    {
        const char* nx = s + 1;
        UzuParserInstruc* instruc = uzucode_parse_getInstruction(a_func, &nx);
        if (instruc != NULL)
        {
            if (strcmp(instruc->Name, FUNCSTART) == 0)
            {
                instruc->ReturnVar = n;

                *a_str = nx;
            }
            else if (strcmp(instruc->Name, COMPARE_INSTRUC) == 0)
            {
                const char* q = strchr(*a_str, '"');
                if (q != NULL && q < nx)
                {
                    const char* qE = strchr(q + 1, '"');
                    assert(qE != NULL);

                    const int len = (int)(qE - q);
                    instruc->ConstantA = malloc(len);
                    memcpy(instruc->ConstantA, q + 1, len);

                    instruc->ConstantA[len - 1] = 0;

                    free(n);
                }
                else
                {
                    instruc->VarA = n;
                }

                q = strchr(nx, '"');
                if (q != NULL)
                {
                    const char* qE = strchr(q + 1, '"');
                    assert(qE != NULL);

                    const int len = (int)(qE - q);
                    instruc->ConstantB = malloc(len);
                    memcpy(instruc->ConstantB, q + 1, len);

                    instruc->ConstantB[len - 1] = 0;

                    *a_str = qE + 1;
                }
                else
                {
                    const char* c = strchr(nx, ' ');

                    const int len = (int)(c - nx);
                    instruc->VarB = malloc(len);
                    for (int i = 0; i < len; ++i)
                    {
                        instruc->VarB[i] = nx[i + 1];
                        if (instruc->VarB[i] == ' ' || instruc->VarA[i] == '\n')
                        {
                            instruc->VarB[i] = 0;

                            break;
                        }
                    }
                    instruc->VarB[len - 1] = 0;

                    *a_str = c;
                }
            }
            else
            {
                free(n);
            }
        }
        else
        {
            free(n);

            *a_str = nx;
        }

        return instruc;
    }

    return NULL;
}
void uzucode_parse_instructions(UzuParserFunc* a_func, const char* a_str)
{
    const char* str = a_str;

    unsigned int index = 0;

    unsigned int jumpStackSize = 0;
    unsigned int* jumpStack = NULL;

    while (str != NULL)
    {
        while (*str == ' ' || *str == '\n' || *str == '\t')
        {
            ++str;
        }

        uzucode_parse_getInstruction(a_func, &str);
    }
}

UzuParser* uzucode_parser_init(const char* a_str)
{
    UzuParser* parser = malloc(sizeof(UzuParser));
    parser->FuncCount = 0;
    parser->Functions = NULL;

    const char* f = strchr(a_str, FUNCSTART[0]);
    assert(f != NULL);

    while (f != NULL)
    {
        if (strncmp(f, FUNCSTART, strlen(FUNCSTART)) == 0)
        {
            const char* fE = strchr(f + 1, FUNCEND[0]);
            assert(fE != NULL);
            while (strncmp(fE, FUNCEND, strlen(FUNCEND)) != 0)
            {
                fE = strchr(fE + 1, FUNCEND[0]);

                if (fE == NULL)
                {
                    break;
                }    
            }

            if (fE != NULL)
            {
                UzuParserFunc* func = malloc(sizeof(UzuParserFunc));
                func->InstructionCount = 0;
                func->Instructions = NULL;

                const char* n = strchr(f, ' ');
                const char* nE = strchr(n + 1, ' ');
                const int len = (int)(nE - n);

                func->Name = malloc(len);
                for (int i = 0; i < len; ++i)
                {
                    func->Name[i] = n[i + 1];
                    if (func->Name[i] == '\n' || func->Name[i] == ' ')
                    {
                        func->Name[i] = 0;

                        break;
                    }
                }
                func->Name[len - 1] = 0;

                if (parser->Functions != NULL)
                {
                    parser->Functions = realloc(parser->Functions, sizeof(UzuParserFunc*) * (parser->FuncCount + 1));
                }
                else
                {
                    parser->Functions = malloc(sizeof(UzuParserFunc*) * (parser->FuncCount + 1));
                }

                parser->Functions[parser->FuncCount] = func;

                ++parser->FuncCount;

                const int bLen = (int)(fE - nE);
                char* body = malloc(bLen);
                memcpy(body, nE + 1, bLen);
                body[bLen - 1] = 0;

                uzucode_parse_instructions(func, body);

                free(body);

                f = fE + strlen(FUNCEND);

                continue;
            }
        }

        f = strchr(f + 1, FUNCSTART[0]);
    }

    return parser;
}

void uzucode_parser_destroy(UzuParser* a_parser)
{
    for (unsigned int i = 0; i < a_parser->FuncCount; ++i)
    {
        UzuParserFunc* func = a_parser->Functions[i];

        for (unsigned int j = 0; j < func->InstructionCount; ++j)
        {
            UzuParserInstruc* instruc = func->Instructions[j];

            free(instruc->Name);
            if (instruc->ConstantA != NULL)
            {
                free(instruc->ConstantA);
            }
            if (instruc->ConstantB != NULL)
            {
                free(instruc->ConstantB);
            }

            if (instruc->VarA != NULL)
            {
                free(instruc->VarA);
            }
            if (instruc->VarB != NULL)
            {
                free(instruc->VarB);
            }
        }

        free(func->Name);
        free(func);
    }

    free(a_parser->Functions);

    free(a_parser);
}