#ifndef UZUPARSER_HEADER
#define UZUPARSER_HEADER

#define FUNCSTART "daydream"
#define FUNCEND "enddaydream"

#define LOOPSTART "guro"
#define LOOPEND "endguro"

#define IFSTART "does"
#define IFEND "enddoes"

#define PRINT_INSTRUC "shout"
#define READ_INSTRUC "megaphone"
#define BREAK_INSTRUC "dream"
#define RETURN_INSTRUC "letsgo"
#define COMPARE_INSTRUC "wuv"

typedef struct 
{
    char* Name;
    char* ReturnVar;
    char* VarA;
    char* VarB;
    char* ConstantA;
    char* ConstantB;
} UzuParserInstruc;

typedef struct 
{
    char* Name;
    unsigned int InstructionCount;
    UzuParserInstruc** Instructions;
} UzuParserFunc;

typedef struct 
{
    unsigned int FuncCount;
    UzuParserFunc** Functions;
} UzuParser;

void uzucode_parse_pushInstruction(UzuParserFunc* a_func, UzuParserInstruc* a_instruc);
UzuParserInstruc* uzucode_parse_pushEmpty(const char* a_name, UzuParserFunc* a_func);

UzuParserInstruc* uzucode_parse_getInstruction(UzuParserFunc* a_func, const char** a_str);
void uzucode_parse_instructions(UzuParserFunc* a_func, const char* a_str);

UzuParser* uzucode_parser_init(const char* a_str);

void uzucode_parser_destroy(UzuParser* a_parser);

#endif