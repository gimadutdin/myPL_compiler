#include <bits/stdc++.h>
using namespace std;

//const int STRING = 0, NUM = 1;
//const int STRING_SIZE = 256;

//command types
const int CMD_PUSH = 5, CMD_POP = 6, CMD_PUSHS = 7, CMD_FETCH = 8, CMD_STORE = 9,
CMD_CALL = 10, CMD_JMP = 11, CMD_JZ = 12, CMD_JNZ = 13, CMD_PLUS = 14, CMD_MINUS = 15,
CMD_MUL = 16, CMD_DIV = 17, CMD_MOD = 18, CMD_AND = 19, CMD_OR = 20, CMD_NOT = 21,
CMD_EQ = 22, CMD_NEQ = 23, CMD_LESS = 24, CMD_GRT = 25, CMD_LEQ = 26, CMD_GEQ = 27;


//-----------------------------------
struct Argument
{
    int type;
    char s_value[STRING_VALUE_SIZE];
    int n_value;
};

struct Command
{
    int type;
    Argument args[2];
};

vector<Command> commands;

void compileBlock(ASTnode *block)
{
    int bt = block->type;
    if (bt == NUM)
    {
        appendPushNumber(block->n_value);
    } else if (bt == STRING)
    {
        appendPushString(block->s_value);
    } else if (bt == ID)
    {
        appendFetch(block->s_value);
    } else if (bt == ASSIGN)
    {
        compileBlock(block->childs[1]);
        appendStore(block->childs[0]);
    } else if (bt == EQ || bt == NEQ || bt == LESS || bt == GRT || bt == LEQ || bt == GEQ || bt == AND || bt == OR || bt == NOT || bt == PLUS || bt == MINUS || bt == MUL || bt == DIV || bt == MOD)
    {
        compileBlock(block->childs[1]);
        compileBlock(block->childs[0]);
        if (bt == EQ) appendEq();
        if (bt == NEQ) appendNeq();
        if (bt == LESS) appendLess();
        if (bt == GRT) appendGrt();
        if (bt == LEQ) appendLeq();
        if (bt == GEQ) appendGeq();
        if (bt == AND) appendAnd();
        if (bt == OR) appendOr();
        if (bt == NOT) appendNot();
        if (bt == PLUS) appendPlus();
        if (bt == MINUS) appendMinus();
        if (bt == MUL) appendMul();
        if (bt == DIV) appendDiv();
        if (bt == MOD) appendMod();
    } else if (bt == FUNC_CALL)
    {
        for (int i = block->child_count - 1; i >= 0; i--)
        {
            if (block->child[i]->type == NUM)
            {
                appendPushNumber(block->child[i]->n_value);
            } else if (block->child[i]->type == STRING)
            {
                appendPushString(block->child[i]->s_value);
            }
        }
        if (strcmp(block->s_value, "input") == 0)
        {
            appendCallInput(2);
        } else if (strcmp(block->s_value, "print") == 0)
        {
            appendCallPrint(block->child_count);
        }
    } else if (bt == BLOCK)
    {
        for (int i = 0; i < block->child_count; i++)
        {
            compileBlock(block->childs[i]);
        }
    } else if (bt == IF)
    {
        compileBlock(block->childs[0]);
        Command jnz;
        jnz.type = CMD_JNZ;
        jnz.args[0] = (int)commands.size() + 3;//number first instruction of if + block
        commands.push_back(jnz);

        Command jz;
        jz.type = CMD_JZ;
        commands.push_back(jz);
        auto jz_ptr = commands.rbegin();

        compileBlock(block->childs[1]);

        Command jmp;
        jmp.type = CMD_JMP;
        commands.push_back(jmp);
        auto jmp_ptr = commands.rbegin();

        (*jz_ptr).args[0] = (int)commands.size() + 1;//number of first instruction after if + block
        if (block->child_count > 1)//if there are else branch
        {
            compileBlock(block->childs[2]);
        }
        (*jmp_ptr).args[0] = (int)commands.size() + 1;//number of first instruction after else block

    } else if (bt == WHILE)
    {
        int cond_addr = commands.size() + 1;
        compileBlock(block->childs[0]);

        Command jz;
        jz.type = CMD_JZ;
        commands.push_back(jz);
        auto jz_ptr = commands.rbegin();

        compileBlock(block->childs[1]);

        Command jmp;
        jmp.type = CMD_JMP;
        jmp.args[0] = cond_addr;
        commands.push_back(jmp);

        (*jz_ptr).args[0] = (int)commands.size() + 1;
    }
}
