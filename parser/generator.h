#ifndef GENERATOR_H_INCLUDED
#define GENERATOR_H_INCLUDED

#include "constants.h"
#include "parser_datastructures.h"

struct Argument
{//int type;
    char s_value[STRING_VALUE_SIZE];
    int n_value;
};
struct Command
{
    int type;
    Argument args[2];
};

extern std::vector<Command> commands;
extern char cmd_s_val[40][30];

void compileBlock(ASTnode *block);
void write_commands_to_binfile(std::ofstream &f);
void init_cmd_s_val();



#endif // GENERATOR_H_INCLUDED
