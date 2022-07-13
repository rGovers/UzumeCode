#ifndef UZUINTERPRETER_HEADER
#define UZUINTERPRETER_HEADER

#include "UzuParser.h"

#define MAIN_FUNC "uzume"

typedef enum
{
    OpCode_PushStack,
    OpCode_PopStack,
    OpCode_Jump,
    OpCode_JumpCond,
    OpCode_Comp,
    OpCode_Ret,
    OpCode_ReadRet,
    OpCode_Print,
    OpCode_Read
} e_OpCode;

typedef enum 
{
    VariableType_Null,
    VariableType_String
} e_VariableType;

typedef struct
{
    e_VariableType Type;
    void* Value;
} UzuInterpreterVar;

typedef struct
{
    unsigned int VarCount;
    UzuInterpreterVar* Variables;
    unsigned int InstructionIndex;
} UzuInterpreterStack;
typedef struct 
{
    e_OpCode OpCode;
    unsigned int VarA;
    unsigned int VarB;
    void* ConstantA;
    void* ConstantB;
    unsigned int OutIndex;
} UzuInterpreterInstruc;

typedef struct 
{
    unsigned int MainIndex;
    unsigned int InstructionCount;
    UzuInterpreterInstruc* Instructions;
} UzuInterpreter;


void uzucode_interpreter_pushFunctionInfo(const UzuParserFunc* a_func, unsigned int a_index, unsigned int* a_infoCount, char*** a_namePtr, unsigned int** a_indexPtr);

void uzucode_interpreter_pushJumpResolution(const char* a_name, unsigned int a_index, unsigned int* a_jumpCount, char*** a_jumpName, unsigned int** a_jumpIndex);
void uzucode_interpreter_pushLJump(unsigned int a_index, unsigned int* a_jumpCount, unsigned int** a_jumpIndex, unsigned int*** a_jumpFrame, unsigned int** a_lJumpFrameSize);
void uzucode_interpreter_pushLJumpBreak(unsigned int a_index, unsigned int* a_frameSize, unsigned int** a_jumpFrame);

void uzucode_interpreter_pushCJump(unsigned int a_index, unsigned int** a_jumpStack, unsigned int* a_stackSize);

void uzucode_interpreter_pushInstruction(UzuInterpreter* a_interpreter, UzuInterpreterInstruc a_instruc);
unsigned int uzucode_interpreter_getVariableIndex(const char* a_name, char*** a_nameLookup, unsigned int* a_nameCount);

void uzucode_interpreter_resizeStackVariable(UzuInterpreterStack* a_stack, unsigned int a_size);
void uzucode_interpreter_pushString(UzuInterpreterStack* a_stack, unsigned int a_index, const char* a_string);

void uzucode_interpreter_pushStack(UzuInterpreterStack** a_stack, unsigned int* a_stackSize);

UzuInterpreter* uzucode_interpreter_init(const UzuParser* a_parser);

void uzucode_interpreter_run(const UzuInterpreter* a_interpreter);

void uzucode_interpreter_destroy(UzuInterpreter* a_interpreter);

#endif