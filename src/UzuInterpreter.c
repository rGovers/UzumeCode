#include "UzuInterpreter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void uzucode_interpreter_pushFunctionInfo(const UzuParserFunc* a_func, unsigned int a_index, unsigned int* a_infoCount, char*** a_namePtr, unsigned int** a_indexPtr)
{
    if (*a_namePtr != NULL)
    {
        *a_namePtr = realloc(*a_namePtr, sizeof(char*) * (*a_infoCount + 1));
    }
    else
    {
        *a_namePtr = malloc(sizeof(char*) * (*a_infoCount + 1));
    }
    (*a_namePtr)[*a_infoCount] = a_func->Name;

    if (*a_indexPtr != NULL)
    {
        *a_indexPtr = realloc(*a_indexPtr, sizeof(unsigned int) * (*a_infoCount + 1));
    }
    else 
    {
        *a_indexPtr = malloc(sizeof(unsigned int) * (*a_infoCount + 1));
    }
    (*a_indexPtr)[*a_infoCount] = a_index;

    ++*a_infoCount;
}

void uzucode_interpreter_pushJumpResolution(const char* a_name, unsigned int a_index, unsigned int* a_jumpCount, char*** a_jumpName, unsigned int** a_jumpIndex)
{
    if (*a_jumpName != NULL)
    {
        *a_jumpName = realloc(*a_jumpName, sizeof(char*) * (*a_jumpCount + 1));
    }
    else
    {
        *a_jumpName = malloc(sizeof(char*) * (*a_jumpCount + 1));
    }
    (*a_jumpName)[*a_jumpCount] = a_name;

    if (*a_jumpIndex != NULL)
    {
        *a_jumpIndex = realloc(*a_jumpIndex, sizeof(unsigned int*) * (*a_jumpCount + 1));
    }
    else
    {
        *a_jumpIndex = malloc(sizeof(unsigned int*) * (*a_jumpCount + 1));
    }
    (*a_jumpIndex)[*a_jumpCount] = a_index;

    ++*a_jumpCount;
}
void uzucode_interpreter_pushLJump(unsigned int a_index, unsigned int* a_jumpCount, unsigned int** a_jumpIndex, unsigned int*** a_frameJump, unsigned int** a_lJumpFrameSize)
{
    if (*a_frameJump != NULL)
    {
        *a_frameJump = realloc(*a_frameJump, sizeof(unsigned int**) * (*a_jumpCount + 1));
    }
    else
    {
        *a_frameJump = malloc(sizeof(unsigned int**) * (*a_jumpCount + 1));
    }
    (*a_frameJump)[*a_jumpCount] = NULL;

    if (*a_lJumpFrameSize != NULL)
    {
        *a_lJumpFrameSize = realloc(*a_lJumpFrameSize, sizeof(unsigned int*) * (*a_jumpCount + 1));
    }
    else
    {
        *a_lJumpFrameSize = malloc(sizeof(unsigned int*) * (*a_jumpCount + 1));
    }
    (*a_lJumpFrameSize)[*a_jumpCount] = 0;

    if (*a_jumpIndex != NULL)
    {
        *a_jumpIndex = realloc(*a_jumpIndex, sizeof(unsigned int) * (*a_jumpCount + 1));
    }
    else
    {
        *a_jumpIndex = malloc(sizeof(unsigned int) * (*a_jumpCount + 1));
    }
    (*a_jumpIndex)[*a_jumpCount] = a_index;

    ++*a_jumpCount;
}
void uzucode_interpreter_pushLJumpBreak(unsigned int a_index, unsigned int* a_frameSize, unsigned int** a_jumpFrame)
{
    if (*a_jumpFrame != NULL)
    {
        *a_jumpFrame = realloc(*a_jumpFrame, sizeof(unsigned int*) * (*a_frameSize + 1));
    }
    else
    {
        *a_jumpFrame = malloc(sizeof(unsigned int) * (*a_frameSize + 1));
    }

    (*a_jumpFrame)[(*a_frameSize)++] = a_index;
}

void uzucode_interpreter_pushCJump(unsigned int a_index, unsigned int** a_jumpStack, unsigned int* a_stackSize)
{
    if (*a_jumpStack != NULL)
    {
        *a_jumpStack = realloc(*a_jumpStack, sizeof(unsigned int) * (*a_stackSize + 1));
    }
    else 
    {
        *a_jumpStack = malloc(sizeof(unsigned int) * (*a_stackSize + 1));
    }

    (*a_jumpStack)[(*a_stackSize)++] = a_index;
}

void uzucode_interpreter_pushInstruction(UzuInterpreter* a_interpreter, UzuInterpreterInstruc a_instruc)
{
    if (a_interpreter->Instructions != NULL)
    {
        a_interpreter->Instructions = realloc(a_interpreter->Instructions, sizeof(UzuInterpreterInstruc) * (a_interpreter->InstructionCount + 1));
    }
    else
    {
        a_interpreter->Instructions = malloc(sizeof(UzuInterpreterInstruc) * (a_interpreter->InstructionCount + 1));
    }

    a_interpreter->Instructions[a_interpreter->InstructionCount++] = a_instruc;
}
unsigned int uzucode_interpreter_getVariableIndex(const char* a_name, char*** a_nameLookup, unsigned int* a_nameCount)
{
    if (*a_nameLookup != NULL)
    {
        for (unsigned int i = 0; i < *a_nameCount; ++i)
        {
            if (strcmp(a_name, (*a_nameLookup)[i]) == 0)
            {
                return i;
            }
        }

        *a_nameLookup = realloc(*a_nameLookup, sizeof(char*) * (*a_nameCount + 1));
    }
    else
    {
        *a_nameLookup = malloc(sizeof(char*) * (*a_nameCount + 1));
    }

    (*a_nameLookup)[*a_nameCount] = a_name;

    return (*a_nameCount)++;
}

UzuInterpreter* uzucode_interpreter_init(const UzuParser* a_parser)
{
    UzuInterpreter* interpreter = malloc(sizeof(UzuInterpreter));
    interpreter->InstructionCount = 0;
    interpreter->Instructions = NULL;
    interpreter->MainIndex = 0;

    unsigned int infoCount = 0;
    char** funcNames = NULL;
    unsigned int* funcIndex = NULL;

    unsigned int jumpCount = 0;
    char** jumpName = NULL;
    unsigned int* jumpIndex = NULL;

    unsigned int index = 0;
    for (unsigned int i = 0; i < a_parser->FuncCount; ++i)
    {
        unsigned int lJumpCount = 0;
        unsigned int* lJumpIndex = NULL;
        unsigned int* lJumpFrameSize = NULL;
        unsigned int** lJumpBreak = NULL;

        unsigned int cJumpCount = 0;
        unsigned int* cJumpIndex = NULL;

        const UzuParserFunc* func = a_parser->Functions[i];
        uzucode_interpreter_pushFunctionInfo(func, index, &infoCount, &funcNames, &funcIndex);

        if (strncmp(func->Name, MAIN_FUNC, strlen(MAIN_FUNC)) == 0)
        {
            interpreter->MainIndex = index;
        }

        unsigned int varCount = 0;
        char** varNames = NULL;

        UzuInterpreterInstruc instruc;
        instruc.OpCode = OpCode_PushStack;
        instruc.ConstantA = NULL;
        instruc.ConstantB = NULL;
        instruc.VarA = -1;
        instruc.VarB = -1;
        instruc.OutIndex = -1;

        uzucode_interpreter_pushInstruction(interpreter, instruc);

        ++index;

        for (unsigned int j = 0; j < func->InstructionCount; ++j)
        {
            const UzuParserInstruc* pInstruc = func->Instructions[j];

            if (strcmp(pInstruc->Name, PRINT_INSTRUC) == 0)
            {
                instruc.OpCode = OpCode_Print;
                instruc.ConstantA = NULL;
                instruc.ConstantB = NULL;
                instruc.VarA = -1;
                instruc.VarB = -1;
                instruc.OutIndex = -1;

                if (pInstruc->ConstantA != NULL)
                {
                    const int len = (int)strlen(pInstruc->ConstantA) + 1;
                    instruc.ConstantA = malloc(len);
                    memcpy(instruc.ConstantA, pInstruc->ConstantA, len);
                }
                else
                {
                    instruc.VarA = uzucode_interpreter_getVariableIndex(pInstruc->VarA, &varNames, &varCount);
                }

                uzucode_interpreter_pushInstruction(interpreter, instruc);
                ++index;    
            }
            else if (strcmp(pInstruc->Name, FUNCSTART) == 0)
            {
                instruc.OpCode = OpCode_Jump;
                instruc.ConstantA = NULL;
                instruc.ConstantB = NULL;
                instruc.VarA = -1;
                instruc.VarB = -1;
                instruc.OutIndex = -1;

                uzucode_interpreter_pushJumpResolution(pInstruc->ConstantA, index, &jumpCount, &jumpName, &jumpIndex);

                uzucode_interpreter_pushInstruction(interpreter, instruc);
                ++index;

                if (pInstruc->ReturnVar != NULL)
                {
                    instruc.OpCode = OpCode_ReadRet;
                    instruc.ConstantA = NULL;
                    instruc.ConstantB = NULL;
                    instruc.VarA = -1;
                    instruc.VarB = -1;

                    instruc.OutIndex = uzucode_interpreter_getVariableIndex(pInstruc->ReturnVar, &varNames, &varCount);

                    uzucode_interpreter_pushInstruction(interpreter, instruc);
                    ++index;
                }
            }
            else if (strcmp(pInstruc->Name, LOOPSTART) == 0)
            {
                uzucode_interpreter_pushLJump(index, &lJumpCount, &lJumpIndex, &lJumpBreak, &lJumpFrameSize);   
            }
            else if (strcmp(pInstruc->Name, LOOPEND) == 0)
            {
                instruc.OpCode = OpCode_Jump;
                instruc.ConstantA = NULL;
                instruc.ConstantB = NULL;
                instruc.VarA = -1;
                instruc.VarB = -1;
                instruc.OutIndex = -1;

                instruc.VarA = lJumpIndex[--lJumpCount];

                uzucode_interpreter_pushInstruction(interpreter, instruc);
                ++index;

                if (lJumpBreak != NULL)
                {
                    const unsigned int frameSize = lJumpFrameSize[lJumpCount];
                    const unsigned int* frame = lJumpBreak[lJumpCount];

                    for (unsigned int i = 0; i < frameSize; ++i)
                    {
                        interpreter->Instructions[frame[i]].VarA = index;
                    }
                }
            }
            else if (strcmp(pInstruc->Name, IFSTART) == 0)
            {
                instruc.OpCode = OpCode_JumpCond;
                instruc.ConstantA = NULL;
                instruc.ConstantB = NULL;
                instruc.VarA = -1;
                instruc.VarB = -1;
                instruc.OutIndex = -1;

                uzucode_interpreter_pushCJump(index, &cJumpIndex, &cJumpCount);
                
                uzucode_interpreter_pushInstruction(interpreter, instruc);
                ++index;
            }
            else if (strcmp(pInstruc->Name, IFEND) == 0)
            {
                interpreter->Instructions[cJumpIndex[cJumpCount - 1]].VarA = index;
            }
            else if (strcmp(pInstruc->Name, COMPARE_INSTRUC) == 0)
            {
                instruc.OpCode = OpCode_Comp;
                instruc.ConstantA = NULL;
                instruc.ConstantB = NULL;
                instruc.VarA = -1;
                instruc.VarB = -1;
                instruc.OutIndex = -1;

                if (pInstruc->ConstantA != NULL)
                {
                    const int len = (int)strlen(pInstruc->ConstantA) + 1;
                    instruc.ConstantA = malloc(len);
                    memcpy(instruc.ConstantA, pInstruc->ConstantA, len);
                }
                else
                {
                    instruc.VarA = uzucode_interpreter_getVariableIndex(pInstruc->VarA, &varNames, &varCount);
                }

                if (pInstruc->ConstantB != NULL)
                {
                    const int len = (int)strlen(pInstruc->ConstantB) + 1;
                    instruc.ConstantB = malloc(len);
                    memcpy(instruc.ConstantB, pInstruc->ConstantB, len);
                }
                else
                {
                    instruc.VarB = uzucode_interpreter_getVariableIndex(pInstruc->VarB, &varNames, &varCount);
                }

                uzucode_interpreter_pushInstruction(interpreter, instruc);
                ++index;
            }
            else if (strcmp(pInstruc->Name, BREAK_INSTRUC) == 0)
            {
                instruc.OpCode = OpCode_Jump;
                instruc.ConstantA = NULL;
                instruc.ConstantB = NULL;
                instruc.VarA = -1;
                instruc.VarB = -1;
                instruc.OutIndex = -1;

                uzucode_interpreter_pushLJumpBreak(index, &(lJumpFrameSize[lJumpCount - 1]), &(lJumpBreak[lJumpCount - 1]));

                uzucode_interpreter_pushInstruction(interpreter, instruc);
                ++index;
            }
            else if (strcmp(pInstruc->Name, RETURN_INSTRUC) == 0)
            {
                instruc.OpCode = OpCode_Ret;
                instruc.ConstantA = NULL;
                instruc.ConstantB = NULL;
                instruc.VarA = -1;
                instruc.VarB = -1;
                instruc.OutIndex = -1;

                if (pInstruc->ConstantA != NULL)
                {
                    const int len = (int)strlen(pInstruc->ConstantA) + 1;
                    instruc.ConstantA = malloc(len);
                    memcpy(instruc.ConstantA, pInstruc->ConstantA, len);
                }
                else
                {
                    instruc.VarA = uzucode_interpreter_getVariableIndex(pInstruc->VarA, &varNames, &varCount);
                }

                uzucode_interpreter_pushInstruction(interpreter, instruc);
                ++index;

                instruc.OpCode = OpCode_PopStack;
                instruc.ConstantA = NULL;
                instruc.ConstantB = NULL;
                instruc.VarA = -1;
                instruc.VarB = -1;
                instruc.OutIndex = -1;

                uzucode_interpreter_pushInstruction(interpreter, instruc);
                ++index;
            }
            else if (strcmp(pInstruc->Name, READ_INSTRUC) == 0)
            {
                instruc.OpCode = OpCode_Read;
                instruc.ConstantA = NULL;
                instruc.ConstantB = NULL;
                instruc.VarA = -1;
                instruc.VarB = -1;

                instruc.OutIndex = uzucode_interpreter_getVariableIndex(pInstruc->VarA, &varNames, &varCount);

                uzucode_interpreter_pushInstruction(interpreter, instruc);
                ++index;
            }
        }

        if (lJumpBreak != NULL)
        {
            for (unsigned int i = 0; i < lJumpCount; ++i)
            {
                if (lJumpBreak[i] != NULL)
                {
                    free(lJumpBreak[i]);
                }
            }

            free(lJumpBreak);
        }
        if (lJumpIndex != NULL)
        {
            free(lJumpFrameSize);
            free(lJumpIndex);
        }
        
        if (cJumpIndex != NULL)
        {
            free(cJumpIndex);
        }

        instruc.OpCode = OpCode_PopStack;
        instruc.ConstantA = NULL;
        instruc.ConstantB = NULL;
        instruc.VarA = -1;
        instruc.VarB = -1;
        instruc.OutIndex = -1;

        uzucode_interpreter_pushInstruction(interpreter, instruc);

        ++index;

        if (varNames != NULL)
        {
            // for (unsigned int i = 0; i < varCount; ++i)
            // {
            //     free(varNames[i]);
            // }

            free(varNames);
        }
    }
    if (jumpCount > 0)
    {
        for (unsigned int i = 0; i < jumpCount; ++i)
        {
            for (unsigned int j = 0; j < infoCount; ++j)
            {
                if (strcmp(jumpName[i], funcNames[j]) == 0)
                {
                    interpreter->Instructions[jumpIndex[i]].VarA = funcIndex[j];

                    break;
                }
            }
        }

        free(jumpName);
        free(jumpIndex);
    }

    if (infoCount > 0)
    {
        free(funcNames);
        free(funcIndex);
    }

    return interpreter;
}

void uzucode_interpreter_resizeStackVariable(UzuInterpreterStack* a_stack, unsigned int a_size)
{
    if (a_stack->Variables != NULL)
    {
        a_stack->Variables = realloc(a_stack->Variables, sizeof(UzuInterpreterVar) * a_size);
    }
    else
    {
        a_stack->Variables = malloc(sizeof(UzuInterpreterVar) * a_size);
    }

    for (int i = a_stack->VarCount; i < a_size; ++i)
    {
        a_stack->Variables[i].Type = VariableType_Null;
        a_stack->Variables[i].Value = NULL;
    }

    a_stack->VarCount = a_size;
}
void uzucode_interpreter_pushString(UzuInterpreterStack* a_stack, unsigned int a_index, const char* a_string)
{
    if (a_stack->VarCount < a_index + 1)
    {
        uzucode_interpreter_resizeStackVariable(a_stack, a_index + 1);
    }
    const int size = (int)strlen(a_string) + 1;

    if (a_stack->Variables[a_index].Value != NULL)
    {
        const int other = (int)strlen(a_stack->Variables[a_index].Value) + 1;

        if (other < size)
        {
            a_stack->Variables[a_index].Value = realloc(a_stack->Variables[a_index].Value, size);
        }
    }
    else
    {
        a_stack->Variables[a_index].Value = malloc(size);
    }

    a_stack->Variables[a_index].Type = VariableType_String;
    memcpy(a_stack->Variables[a_index].Value, a_string, size);
}

void uzucode_interpreter_pushStack(UzuInterpreterStack** a_stack, unsigned int* a_stackSize)
{
    if (*a_stack != NULL)
    {
        *a_stack = realloc(*a_stack, sizeof(UzuInterpreterStack) * (*a_stackSize + 1));
    }
    else
    {
        *a_stack = malloc(sizeof(UzuInterpreterStack) * (*a_stackSize + 1));
    }

    (*a_stack)[*a_stackSize].VarCount = 0;
    (*a_stack)[*a_stackSize].Variables = NULL;
    (*a_stack)[*a_stackSize].InstructionIndex = 0;

    ++*a_stackSize;
}

void uzucode_interpreter_run(const UzuInterpreter* a_interpreter)
{
    printf("Starting Execution \n");
    printf("----------------------- \n");
    printf("\n");

    unsigned int iIndex = a_interpreter->MainIndex;

    unsigned int stackSize = 0;
    UzuInterpreterStack* stack = NULL;

    UzuInterpreterVar retVar;
    retVar.Type = VariableType_Null;
    retVar.Value = NULL;

    unsigned result;

    unsigned char exit = 0;

    while (exit == 0)
    {
        switch (a_interpreter->Instructions[iIndex].OpCode)
        {
        case OpCode_PushStack:
        {
#ifndef NDEBUG
            printf("Pushing callstack: ins.%d \n", iIndex);
#endif

            retVar.Type = VariableType_Null;
            if (retVar.Value != NULL)
            {
                free(retVar.Value);
                retVar.Value = NULL;
            }

            uzucode_interpreter_pushStack(&stack, &stackSize);

            break;
        }
        case OpCode_PopStack:
        {
#ifndef NDEBUG
            printf("Poping callstack: int.%d \n", iIndex);
#endif

            --stackSize;
            exit = stackSize <= 0;

            UzuInterpreterStack s = stack[stackSize];

            if (s.Variables != NULL)
            {
                for (unsigned int i = 0; i < s.VarCount; ++i)
                {
                    if (s.Variables[i].Value != NULL)
                    {
                        free(s.Variables[i].Value);
                    }
                }

                free(s.Variables);
            }
            if (stackSize >= 1)
            {
                iIndex = stack[stackSize - 1].InstructionIndex;
            }

            break;
        }
        case OpCode_Jump:
        {
            const unsigned int jInd = a_interpreter->Instructions[iIndex].VarA - 1;
#ifndef NDEBUG
            printf("Jumping to insruction: ins.%d->%d \n", iIndex, jInd + 1);
#endif

            stack[stackSize - 1].InstructionIndex = iIndex;

            if (jInd != -2)
            {
                iIndex = jInd;
            }
            else
            {
                printf("Invalid jump instruction \n");
            }

            break;
        }
        case OpCode_JumpCond:
        {
#ifndef NDEBUG
            printf("Hit conditional jump: ins.%d \n", iIndex);
#endif

            if (result == 0)
            {
                const unsigned int jInd = a_interpreter->Instructions[iIndex].VarA - 1;
#ifndef NDEBUG
                printf("Jumping to insruction: ins.%d->%d \n", iIndex, jInd + 1);
#endif
                iIndex = jInd;
            }

            break;
        }
        case OpCode_Comp:
        {
            const char* valA = a_interpreter->Instructions[iIndex].ConstantA;
            if (valA == NULL)
            {
                valA = stack[stackSize - 1].Variables[a_interpreter->Instructions[iIndex].VarA].Value;
            }

            const char* valB = a_interpreter->Instructions[iIndex].ConstantB;
            if (valB == NULL)
            {
                valB = stack[stackSize - 1].Variables[a_interpreter->Instructions[iIndex].VarB].Value;
            }

            result = strcmp(valA, valB) == 0;

            break;
        }
        case OpCode_Ret:
        {
#ifndef NDEBUG
            printf("Pushing return value: ins.%d \n", iIndex);
#endif

            const char* cos = a_interpreter->Instructions[iIndex].ConstantA;
            if (cos != NULL)
            {
                retVar.Type = VariableType_String;

                const int len = (int)strlen(cos) + 1;
                retVar.Value = malloc(len);
                memcpy(retVar.Value, cos, len);
            }
            else
            {
                UzuInterpreterVar var = stack[stackSize - 1].Variables[a_interpreter->Instructions[iIndex].VarA];
                retVar.Type = var.Type;

                const int len = (int)strlen(var.Value) + 1;
                retVar.Value = malloc(len);
                memcpy(retVar.Value, var.Value, len);
            }

            break;
        }
        case OpCode_ReadRet:
        {
#ifndef NDEBUG
            printf("Moving return value: ins.%d \n", iIndex);
#endif

            const unsigned int index = a_interpreter->Instructions[iIndex].OutIndex;
            if (stack[stackSize - 1].VarCount < index + 1)
            {
                uzucode_interpreter_resizeStackVariable(&stack[stackSize - 1], index + 1);
            } 

            stack[stackSize - 1].Variables[index].Type = retVar.Type;

            if (stack[stackSize - 1].Variables[index].Value != NULL)
            {
                free(stack[stackSize - 1].Variables[index].Value);
            }

            stack[stackSize - 1].Variables[index].Value = retVar.Value;

            retVar.Type = VariableType_Null;
            retVar.Value = NULL;

            break;
        }
        case OpCode_Print:
        {
            const char* cos = a_interpreter->Instructions[iIndex].ConstantA;
            if (cos != NULL)
            {
                printf("> %s <\n", cos);
            }
            else
            {
                const char* str = stack[stackSize - 1].Variables[a_interpreter->Instructions[iIndex].VarA].Value;
                if (str != NULL)
                {
                    printf("> %s <\n", str);
                }
                else
                {
                    printf("Var error \n");
                }
            }

            break;
        }
        case OpCode_Read:
        {
            char buff[1024];

            printf("~ ");
            scanf("%1000s", buff);

            uzucode_interpreter_pushString(&(stack[stackSize - 1]), a_interpreter->Instructions[iIndex].OutIndex, buff);

            break;
        }
        }

        if (exit)
        {
            break;
        }

        if (stackSize <= 0)
        {
            printf("Stack error \n");

            break;
        }

        if (++iIndex > a_interpreter->InstructionCount)
        {
            printf("Instruction execution error \n");

            break;
        }
    }

    if (stack != NULL)
    {
        free(stack);
    }

    printf("\n");
    printf("----------------------- \n");
    printf("Exiting \n");
}

void uzucode_interpreter_destroy(UzuInterpreter* a_interpreter)
{
    if (a_interpreter->Instructions != NULL)
    {
        for (unsigned int i = 0; i < a_interpreter->InstructionCount; ++i)
        {
            if (a_interpreter->Instructions[i].ConstantA != NULL)
            {
                free(a_interpreter->Instructions[i].ConstantA);
            }

            if (a_interpreter->Instructions[i].ConstantB != NULL)
            {
                free(a_interpreter->Instructions[i].ConstantB);
            }
        }

        free(a_interpreter->Instructions);
    }

    free(a_interpreter);
}