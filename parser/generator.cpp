#include <bits/stdc++.h>
#include "generator.h"
//#include "constants.h"
//#include "parser_datastructures.h"
using namespace std;

vector<Command> commands;
char cmd_s_val[40][30];

void appendPushNumber(int num)
{
    Command cmd;
    cmd.type = CMD_PUSH;
    cmd.args[0].n_value = num;
    commands.push_back(cmd);
}

void appendPushString(char *str)
{
    Command cmd;
    cmd.type = CMD_PUSHS;
    strcpy(cmd.args[0].s_value, str);
    commands.push_back(cmd);
}

void appendFetch(char *str)
{
    Command cmd;
    cmd.type = CMD_FETCH;
    strcpy(cmd.args[0].s_value, str);
    commands.push_back(cmd);
}

void appendStore(char *str)
{
    Command cmd;
    cmd.type = CMD_STORE;
    strcpy(cmd.args[0].s_value, str);
    commands.push_back(cmd);
}

void init_cmd_s_val()
{
    strcpy(cmd_s_val[CMD_PUSH], "push");
    strcpy(cmd_s_val[CMD_POP], "pop");
    strcpy(cmd_s_val[CMD_PUSHS], "pushs");
    strcpy(cmd_s_val[CMD_FETCH], "fetch");
    strcpy(cmd_s_val[CMD_STORE], "store");
    strcpy(cmd_s_val[CMD_CALL], "call");
    strcpy(cmd_s_val[CMD_JMP], "jmp");
    strcpy(cmd_s_val[CMD_JZ], "jz");
    strcpy(cmd_s_val[CMD_JNZ], "jnz");
    strcpy(cmd_s_val[CMD_PLUS], "plus");
    strcpy(cmd_s_val[CMD_MINUS], "minus");
    strcpy(cmd_s_val[CMD_MUL], "mul");
    strcpy(cmd_s_val[CMD_DIV], "div");
    strcpy(cmd_s_val[CMD_MOD], "mod");
    strcpy(cmd_s_val[CMD_AND], "and");
    strcpy(cmd_s_val[CMD_OR], "or");
    strcpy(cmd_s_val[CMD_NOT], "not");
    strcpy(cmd_s_val[CMD_EQ], "eq");
    strcpy(cmd_s_val[CMD_NEQ], "neq");
    strcpy(cmd_s_val[CMD_LESS], "less");
    strcpy(cmd_s_val[CMD_GRT], "grt");
    strcpy(cmd_s_val[CMD_LEQ], "leq");
    strcpy(cmd_s_val[CMD_GEQ], "geq");
}

void appendEq() {Command cmd; cmd.type = CMD_EQ; commands.push_back(cmd);}
void appendNeq() {Command cmd; cmd.type = CMD_NEQ; commands.push_back(cmd);}
void appendLess() {Command cmd; cmd.type = CMD_LESS; commands.push_back(cmd);}
void appendGrt() {Command cmd; cmd.type = CMD_GRT; commands.push_back(cmd);}
void appendLeq() {Command cmd; cmd.type = CMD_LEQ; commands.push_back(cmd);}
void appendGeq() {Command cmd; cmd.type = CMD_GEQ; commands.push_back(cmd);}
void appendAnd() {Command cmd; cmd.type = CMD_AND; commands.push_back(cmd);}
void appendOr() {Command cmd; cmd.type = CMD_OR; commands.push_back(cmd);}
void appendNot() {Command cmd; cmd.type = CMD_NOT; commands.push_back(cmd);}
void appendPlus() {Command cmd; cmd.type = CMD_PLUS; commands.push_back(cmd);}
void appendMinus() {Command cmd; cmd.type = CMD_MINUS; commands.push_back(cmd);}
void appendMul() {Command cmd; cmd.type = CMD_MUL; commands.push_back(cmd);}
void appendDiv() {Command cmd; cmd.type = CMD_DIV; commands.push_back(cmd);}
void appendMod() {Command cmd; cmd.type = CMD_MOD; commands.push_back(cmd);}

void appendFuncCall(char *func_name, int arg_count)
{
    Command cmd;
    cmd.type = CMD_CALL;
    strcpy(cmd.args[0].s_value, func_name);
    cmd.args[1].n_value = arg_count;
    commands.push_back(cmd);
}


void write_commands_to_binfile(ofstream &f)
{
    int cnt = (int)commands.size();
    f.write((char *)&cnt, sizeof(int));
    for (auto it = commands.begin(); it != commands.end(); it++)
    {
        f.write((char *)&(*it), sizeof(Command));
    }
}


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
        appendStore(block->childs[0]->s_value);
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
            if (block->childs[i]->type == NUM)
            {
                appendPushNumber(block->childs[i]->n_value);
            } else if (block->childs[i]->type == STRING)
            {
                appendPushString(block->childs[i]->s_value);
            } else if (block->childs[i]->type == ID)
            {
                if (strcmp(block->s_value, "print") == 0)
                    appendFetch(block->childs[i]->s_value);
                else//input
                    appendPushString(block->childs[i]->s_value);
            } else//expr
            {
                compileBlock(block->childs[i]);
            }
        }
        if (strcmp(block->s_value, "input") == 0)
        {
            appendFuncCall(block->s_value, 2);
        } else if (strcmp(block->s_value, "print") == 0)
        {
            appendFuncCall(block->s_value, block->child_count);
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
        jnz.args[0].n_value = (int)commands.size() + 2;//number first instruction of if + block
        commands.push_back(jnz);

        Command jz;
        jz.type = CMD_JZ;
        commands.push_back(jz);
        int jz_ind = (int)commands.size() - 1;

        compileBlock(block->childs[1]);

        Command jmp;
        jmp.type = CMD_JMP;
        commands.push_back(jmp);
        int jmp_ind = (int)commands.size() - 1;

        commands[jz_ind].args[0].n_value = (int)commands.size();//number of first instruction after if + block
        if (block->child_count > 1)//if there are else branch
        {
            compileBlock(block->childs[2]);
        }
        commands[jmp_ind].args[0].n_value = (int)commands.size();//number of first instruction after else block

    } else if (bt == WHILE)
    {
        int cond_addr = commands.size();
        compileBlock(block->childs[0]);

        Command jz;
        jz.type = CMD_JZ;
        commands.push_back(jz);
        auto jz_ind = (int)commands.size() - 1;

        compileBlock(block->childs[1]);

        Command jmp;
        jmp.type = CMD_JMP;
        jmp.args[0].n_value = cond_addr;
        commands.push_back(jmp);

        commands[jz_ind].args[0].n_value = (int)commands.size();
    }
}
