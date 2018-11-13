#include <bits/stdc++.h>
using namespace std;

const int STRING = 0, NUM = 1;
const int STRING_SIZE = 256;

// Generator's constants : command types
const int CMD_PUSH = 5, CMD_POP = 6, CMD_PUSHS = 7, CMD_FETCH = 8, CMD_STORE = 9,
CMD_CALL = 10, CMD_JMP = 11, CMD_JZ = 12, CMD_JNZ = 13, CMD_PLUS = 14, CMD_MINUS = 15,
CMD_MUL = 16, CMD_DIV = 17, CMD_MOD = 18, CMD_AND = 19, CMD_OR = 20, CMD_NOT = 21,
CMD_EQ = 22, CMD_NEQ = 23, CMD_LESS = 24, CMD_GRT = 25, CMD_LEQ = 26, CMD_GEQ = 27;
//-------------------------------------------------
struct Argument
{
    char s_value[STRING_SIZE];
    int n_value;
};

struct Command
{
    int type;
    Argument args[2];
};

struct StackNode
{
    int type;//STRING or NUM
    char s_value[STRING_SIZE];
    int n_value;
};

//-------------------------------------------------
stack<StackNode> stk;
vector<Command> commands;
map<string, int> variables;
char cmd_s_val[40][30];


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

void read_commands_file(ifstream &f)
{
    int cnt;
    f.read((char *)&cnt, sizeof(int));
    Command cmd;
    for (int i = 0; i < cnt; i++)
    {
        f.read((char *)&cmd, sizeof(Command));
        commands.push_back(cmd);
    }
}

void show_commands()
{
    Command cur_cmd;
    for (int i = 0; i < (int)commands.size(); i++)
    {
        cur_cmd = commands[i];
        cout << i << ") ";
        cout << cmd_s_val[cur_cmd.type] << " ";
        if (cur_cmd.type == CMD_PUSHS || cur_cmd.type == CMD_FETCH || cur_cmd.type == CMD_STORE || cur_cmd.type == CMD_CALL)
        {
            cout << cur_cmd.args[0].s_value << " ";
            if (cur_cmd.type == CMD_CALL)
            {
                cout << cur_cmd.args[1].n_value << " ";
            }
        } else if (cur_cmd.type == CMD_JZ || cur_cmd.type == CMD_JNZ || cur_cmd.type == CMD_JMP || cur_cmd.type == CMD_PUSH)
        {
            cout << cur_cmd.args[0].n_value << " ";
        }
        cout << "\n";
    }
}

void runCode()
{
    int cmd_count = (int)commands.size() + 1;
    int ip = 0;//instruction pointer
    Command cmd;

    while (ip < cmd_count - 1)
    {
        //cout << ip << "\n";
        cmd = commands[ip];
        if (cmd.type == CMD_PUSH)
        {
            StackNode sn;
            sn.type = NUM;
            sn.n_value = cmd.args[0].n_value;
            stk.push(sn);
            ip++;
        } else if (cmd.type == CMD_PUSHS)
        {
            StackNode sn;
            sn.type = STRING;
            strcpy(sn.s_value, cmd.args[0].s_value);
            stk.push(sn);
            ip++;
        } else if (cmd.type == CMD_POP)
        {
            stk.pop();
            ip++;
        } else if (cmd.type == CMD_FETCH)
        {
            string var_name = string(cmd.args[0].s_value);
            if (variables.count(var_name) > 0)
            {
                StackNode sn;
                sn.type = NUM;
                sn.n_value = variables[var_name];
                stk.push(sn);
            } else
            {
                cout << "Error: variable named " << var_name << " was not found";
                exit(-1);
            }
            ip++;
        } else if (cmd.type == CMD_STORE)
        {
            variables[string(cmd.args[0].s_value)] = stk.top().n_value;
            stk.pop();
            ip++;
        } else if (cmd.type == CMD_JMP)
        {
            ip = cmd.args[0].n_value;
        } else if (cmd.type == CMD_JZ)
        {
            if (stk.top().n_value == 0)
            {
                ip = cmd.args[0].n_value;
                stk.pop();
            } else
            {
                ip++;
            }
        } else if (cmd.type == CMD_JNZ)
        {
            if (stk.top().n_value != 0)
            {
                ip = cmd.args[0].n_value;
                stk.pop();
            } else
            {
                ip++;
            }
        } else if (cmd.type == CMD_PLUS || cmd.type == CMD_MINUS || cmd.type == CMD_MUL || cmd.type == CMD_DIV || cmd.type == CMD_MOD || cmd.type == CMD_AND || cmd.type == CMD_OR || cmd.type == CMD_EQ || cmd.type == CMD_NEQ || cmd.type == CMD_LESS || cmd.type == CMD_GRT || cmd.type == CMD_LEQ || cmd.type == CMD_GEQ)
        {
            int opd1 = stk.top().n_value;
            stk.pop();
            int opd2 = stk.top().n_value;
            stk.pop();
            StackNode sn;
            sn.type = NUM;
            if (cmd.type == CMD_PLUS)
            {
                sn.n_value = opd1 + opd2;
            } else if (cmd.type == CMD_MINUS)
            {
                sn.n_value = opd1 - opd2;
            } else if (cmd.type == CMD_MUL)
            {
                sn.n_value = opd1*opd2;
            } else if (cmd.type == CMD_DIV)
            {
                sn.n_value = opd1/opd2;
            } else if (cmd.type == CMD_MOD)
            {
                sn.n_value = opd1%opd2;
            } else if (cmd.type == CMD_AND)
            {
                sn.n_value = (int)((opd1 != 0) && (opd2 != 0));
            } else if (cmd.type == CMD_OR)
            {
                sn.n_value = (int)((opd1 != 0) || (opd2 != 0));
            } else if (cmd.type == CMD_EQ)
            {
                sn.n_value = (int)(opd1 == opd2);
            } else if (cmd.type == CMD_NEQ)
            {
                sn.n_value = (int)(opd1 != opd2);
            } else if (cmd.type == CMD_LESS)
            {
                sn.n_value = (int)(opd1 < opd2);
            } else if (cmd.type == CMD_GRT)
            {
                sn.n_value = (int)(opd1 > opd2);
            } else if (cmd.type == CMD_LEQ)
            {
                sn.n_value = (int)(opd1 <= opd2);
            } else if (cmd.type == CMD_GEQ)
            {
                sn.n_value = (int)(opd1 >= opd2);
            }
            stk.push(sn);
            ip++;
        } else if (cmd.type == CMD_NOT)
        {
            int opd = stk.top().n_value;
            StackNode sn;
            sn.type = NUM;
            sn.n_value = (int)(!(opd != 0));
            ip++;
        } else if (cmd.type == CMD_CALL)
        {
            if (strcmp(cmd.args[0].s_value, "input") == 0)
            {//arg count = cmd.args[1].n_value
                cout << stk.top().s_value;
                stk.pop();
                string var_name = stk.top().s_value;
                stk.pop();
                int val;
                cin >> val;
                variables[var_name] = val;
            } else if (strcmp(cmd.args[0].s_value, "print") == 0)
            {
                int arg_count = cmd.args[1].n_value;
                StackNode sn;
                for (int i = 0; i < arg_count; i++)
                {
                    sn = stk.top();
                    stk.pop();
                    if (sn.type == NUM)
                    {
                        cout << sn.n_value;
                    } else//string
                    {
                        cout << sn.s_value;
                    }
                }
                cout << "\n";
            }
            ip++;
        }
    }
}

int main()
{
    init_cmd_s_val();
    ifstream commands_bin("commands.bin", ios::binary);
    read_commands_file(commands_bin);
    show_commands();
    runCode();

    system("pause>nul");
    return 0;
}
