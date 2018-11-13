/*
    Parser for myPL
    Author: Rustem Gimadutdinov
*/

#include <bits/stdc++.h>
using namespace std;

//------------------------------------------------------------------------
//    lexer's constants(token types):
const int STRING = 0, NUM = 1, ID = 2, IF = 3, ELSE = 4, WHILE = 5, LBRA = 6, RBRA = 7, LPAR = 8,
RPAR = 9, PLUS = 10, MINUS = 11, MUL = 12, DIV = 13, MOD = 14, EQ = 15, NEQ = 16, LESS = 17, GRT = 18, LEQ = 19,
GEQ = 20, NOT = 21, AND = 22, OR = 23, ASSIGN = 24, SEMICOLON = 25, COMMA = 26, ONE_QUOTE = 27;
//    Additional constants (AST node types):
const int BLOCK = 40, FUNC_CALL = 41;
//-------------------------------------------------------------------------

const int STRING_VALUE_SIZE = 256;
const int MAX_CHILD_1 = 30;
//-------------------------------------------------------------------------
char s_val[50][64];
int priority[40];

void init_s_val()
{
    strcpy(s_val[IF], "if");
    strcpy(s_val[ELSE], "else");
    strcpy(s_val[WHILE], "while");
    strcpy(s_val[LBRA], "{");
    strcpy(s_val[RBRA], "}");
    strcpy(s_val[LPAR], "(");
    strcpy(s_val[RPAR], ")");
    strcpy(s_val[PLUS], "+");
    strcpy(s_val[MINUS], "-");
    strcpy(s_val[MUL], "*");
    strcpy(s_val[DIV], "/");
    strcpy(s_val[MOD], "%");
    strcpy(s_val[EQ], "==");
    strcpy(s_val[NEQ], "!=");
    strcpy(s_val[LESS], "<");
    strcpy(s_val[GRT], ">");
    strcpy(s_val[LEQ], "<=");
    strcpy(s_val[GEQ], ">=");
    strcpy(s_val[NOT], "!");
    strcpy(s_val[AND], "&&");
    strcpy(s_val[OR], "||");
    strcpy(s_val[ASSIGN], "=");
    strcpy(s_val[SEMICOLON], ";");
    strcpy(s_val[COMMA], ",");
    strcpy(s_val[ONE_QUOTE], "'");

    strcpy(s_val[STRING], "STRING");
    strcpy(s_val[NUM], "NUM");
    strcpy(s_val[ID], "ID");
    strcpy(s_val[PLUS], "PLUS");
    strcpy(s_val[MINUS], "MINUS");
    strcpy(s_val[MUL], "MUL");
    strcpy(s_val[DIV], "DIV");
    strcpy(s_val[NOT], "NOT");
    strcpy(s_val[OR], "OR");
    strcpy(s_val[AND], "AND");

    strcpy(s_val[BLOCK], "BLOCK");
    strcpy(s_val[FUNC_CALL], "FUNC_CALL");
}

void init_priorities()
{
    priority[LPAR] = 1;
    priority[OR] = 2;
    priority[AND] = 3;
    priority[EQ] = 4;
    priority[NEQ] = 4;
    priority[LESS] = 5;
    priority[GRT] = 5;
    priority[LEQ] = 5;
    priority[GEQ] = 5;
    priority[PLUS] = 6;
    priority[MINUS] = 6;//so lexer must recognize unar minuses itself
    priority[MUL] = 7;
    priority[DIV] = 7;
    priority[MOD] = 7;
    priority[NOT] = 8;

//    priority[] = ;
//    priority[] = ;
//    priority[] = ;
//    priority[] = ;
}

struct Token
{
    int type;
    int n_value;
    char s_value[STRING_VALUE_SIZE];
};

struct ASTnode
{
    int type;
    int n_value;
    int child_count;
    int max_childs;

    char s_value[STRING_VALUE_SIZE];
    ASTnode** childs;

    ASTnode()
    {
        this->child_count = 0;
        this->max_childs = MAX_CHILD_1;
        childs = new ASTnode*[max_childs];
    }

    void push_child(ASTnode *ptr)
    {
        if (child_count == max_childs)
        {
            this->max_childs += 10;
            ASTnode **tmp = (ASTnode**)realloc(this->childs, this->max_childs*sizeof(ASTnode*));
            if (tmp != NULL)
            {
                this->childs = tmp;
            } else
            {
                cout << "Error reallocating array";
            }
        }
        this->childs[this->child_count] = ptr;
        this->child_count++;
    }
    //dont forget to write a destructor))
};
//------------------------------------------------------------------------
vector<Token> tokens;
int tok_index, token_count;
//------------------------------------------------------------------------
void init()
{
    tok_index = 0;
}
//------------------------------------------------------------------------

bool isArLogOp(Token &t)
{
    return (t.type == PLUS || t.type == MINUS || t.type == MUL || t.type == DIV || t.type == NOT || t.type == OR || t.type == AND);
}

bool isCompOp(Token &t)
{
    return (t.type == EQ || t.type == NEQ || t.type == LESS || t.type == GRT || t.type == LEQ || t.type == GEQ);
}

//maybe and EOF type token ? (it will be the last element)

ASTnode* make_ast_node(Token &t)
{
    ASTnode* a = new ASTnode();
    a->type = t.type;
    a->n_value = t.n_value;
    strcpy(a->s_value, t.s_value);
    return a;
}

ASTnode* parse_expr(int pos, int &end_pos, int stop_pos = -1)
{
    //cout << tokens[pos].type << "\n";

    stack<Token> stack1;
    vector<Token> rpn;
    Token cur_tok;
    do
    {
        if (pos == stop_pos)
        {
            break;
        }
        cur_tok = tokens[pos];
        //cout << cur_tok.type << "\n";////////////
        if (cur_tok.type == ID || cur_tok.type == NUM)
        {
            rpn.push_back(cur_tok);
        }
        else if (isArLogOp(cur_tok) || isCompOp(cur_tok))
        {
            if (stack1.empty() || priority[cur_tok.type] > priority[stack1.top().type])
            {
                stack1.push(cur_tok);
            }
            else
            {
                while (!stack1.empty() && priority[cur_tok.type] <= priority[stack1.top().type])
                {
                    rpn.push_back(stack1.top());
                    stack1.pop();
                }
                stack1.push(cur_tok);
            }
        }
        else if (cur_tok.type == LPAR)
        {
            stack1.push(cur_tok);
        }
        else if (cur_tok.type == RPAR)
        {
            while (!stack1.empty() && stack1.top().type != LPAR)
            {
                rpn.push_back(stack1.top());
                stack1.pop();
            }
            if (!stack1.empty())
                stack1.pop();
        }
        //cout << "pos = " << s_val[tokens[pos].type] << "]\n";
        pos++;
    } while (cur_tok.type == LPAR || cur_tok.type == RPAR || isArLogOp(cur_tok) || isCompOp(cur_tok) || cur_tok.type == NUM || cur_tok.type == ID);

    //cout << "rpn size = " << rpn.size() << "\n";

    while (!stack1.empty())
    {
        rpn.push_back(stack1.top());
        stack1.pop();
    }
//    cout << "((";
//    for (auto it = rpn.begin(); it != rpn.end(); it++) cout << s_val[it->type] << " ";
//    cout << "))\n";
    //building an ast from single ast nodes in stack2
    stack<ASTnode*> ast;
    Token t;
    for (int i = 0; i < (int)rpn.size(); i++)
    {
        t = rpn[i];
        if (t.type == NUM || t.type == ID)
        {
            ast.push(make_ast_node(t));
        } else if (isArLogOp(t) || isCompOp(t))
        {
            if ((int)ast.size() <= 1)
            {
                if (t.type == MINUS)
                    (*ast.top()).n_value *= -1;
                if (t.type == NOT)
                    (*ast.top()).n_value = !(*ast.top()).n_value;
            } else
            {
                ASTnode *tmp = new ASTnode();
                tmp->type = t.type;

                ASTnode *a, *b;
                b = ast.top();
                ast.pop();
                a = ast.top();
                ast.pop();
                tmp->push_child(a);
                tmp->push_child(b);
                ast.push(tmp);
            }
        }
    }//end for
    end_pos = pos - 1;//because do..while
    return ast.top();
}//end parse_expr


//---------------------------------------------------------------------------------
ASTnode* parse_block(int pos, int &endp, int only_one_statement = 0)
{
    ASTnode *block = new ASTnode();
    block->type = BLOCK;
    while (pos < tokens.size() - 1 && tokens[pos].type != RBRA)
    {
        if (tokens[pos].type == SEMICOLON)
        {
            pos++;
        }
        Token cur_tok = tokens[pos];
        //cout << "[[[[" << s_val[cur_tok.type] << "\n";
        if (cur_tok.type == ID)
        {
            if (pos + 1 >= token_count)
            {//pos + 1 - number of current token in numeration starting with 1
                cout << "Error at token " << pos + 1 << " unexpected end of file.";
                return NULL;
            }
            if (tokens[pos + 1].type == ASSIGN)
            {
                ASTnode *asgn, *l_id, *expr;
                asgn = new ASTnode();
                asgn->type = ASSIGN;
                l_id = new ASTnode();
                l_id->type = ID;
                strcpy(l_id->s_value, cur_tok.s_value);
                asgn->push_child(l_id);
                int end_pos;
                expr = parse_expr(pos + 2, end_pos);
                if (expr != NULL)
                {
                    if (tokens[end_pos].type == SEMICOLON)
                    {
                        asgn->push_child(expr);
                        block->push_child(asgn);
                    } else
                    {
                        cout << "Error at token " << end_pos << " semicolon expected";
                    }

                } else
                {
                    cout << "Error at token " << pos + 2 << " after = expression expected";
                    return NULL;
                }
                pos = end_pos + 1;
            } else if (tokens[pos + 1].type == LPAR)
            {
                ASTnode *func = new ASTnode();
                func->type = FUNC_CALL;
                strcpy(func->s_value, cur_tok.s_value);
                pos += 2;
                int i = pos;
                while (tokens[i].type != SEMICOLON)
                {
                    i++;
                }
                int stop_pos = i - 1;
                do
                {//string or expr
                    if (tokens[pos].type == COMMA)
                    {
                        pos++;
                    }
                    if (tokens[pos].type == STRING)
                    {
                        ASTnode *str = new ASTnode();
                        str->type = STRING;
                        strcpy(str->s_value, tokens[pos].s_value);
                        func->push_child(str);
                        pos++;
                    } else//expr
                    {
                        int end_pos;
                        ASTnode *expr = parse_expr(pos, end_pos, stop_pos);
                        //cout << "[[[[[[[[[[[" << s_val[tokens[pos].type] << "\n";
                        if (expr != NULL)
                        {
                            func->push_child(expr);
                        } else
                        {
                            cout << "Unexpected token " << end_pos << "\n";
                            return NULL;
                        }
                        pos = end_pos + 1;
                        if (end_pos + 1 == stop_pos)
                        {
                            break;
                        }
                    }
                } while (tokens[pos].type != RPAR);
                if (tokens[pos + 1].type == SEMICOLON)
                {
                    block->push_child(func);
                    pos++;
                } else
                {
                    cout << "Error at token " << pos + 1 << " ; expected";
                    return NULL;
                }
            }
        } else if (cur_tok.type == IF)
        {
            ASTnode *if_stat, *cond;

            if_stat = new ASTnode();
            if_stat->type = IF;

            int i = pos + 1, balance = 0;
            do
            {
                if (tokens[i].type == LPAR)
                {
                    balance++;
                } else if (tokens[i].type == RPAR)
                {
                    balance--;
                }
                i++;
            } while (balance != 0);
            int stop_pos = i - 1, end_pos;
            cond = parse_expr(pos + 2, end_pos, stop_pos);
            if_stat->push_child(cond);
            block->push_child(if_stat);
            pos = stop_pos + 1;//or end_pos + 2

            if (tokens[pos].type == LBRA)
            {
                int end_pos;
                if_stat->push_child(parse_block(pos + 1, end_pos));
                pos = end_pos + 1;//end_pos - }
                //cout << "[[[[[[" << s_val[tokens[end_pos].type] << "]]]]]]\n";
            } else
            {
                int end_pos;
                if_stat->push_child(parse_block(pos, end_pos, 1));
                pos = end_pos;
                if (tokens[pos].type == SEMICOLON)
                {
                    pos++;
                }
                //cout << "aa[[[[[[" << s_val[tokens[end_pos].type] << "]]]]]]\n";
            }
            if (tokens[pos].type == ELSE)
            {
                pos++;
                if (tokens[pos].type == LBRA)
                {
                    int end_pos;
                    if_stat->push_child(parse_block(pos + 1, end_pos));
                    pos = end_pos + 1;//end_pos - }
                    //cout << "[[[[[[" << s_val[tokens[end_pos].type] << "]]]]]]\n";
                } else
                {
                    int end_pos;
                    if_stat->push_child(parse_block(pos, end_pos, 1));
                    pos = end_pos;
                    //cout << "[[[[[[" << s_val[tokens[end_pos].type] << "]]]]]]\n";
                }
            }//ifELSE
        } else if (cur_tok.type == WHILE)
        {
            ASTnode *while_stat, *cond;
            while_stat = new ASTnode();
            while_stat->type = WHILE;
            int i = pos + 1, balance = 0;
            do
            {
                if (tokens[i].type == LPAR)
                {
                    balance++;
                } else if (tokens[i].type == RPAR)
                {
                    balance--;
                }
                i++;
            } while (balance != 0);
            int stop_pos = i - 1, end_pos;
            cond = parse_expr(pos + 2, end_pos, stop_pos);
            while_stat->push_child(cond);
            block->push_child(while_stat);
            pos = stop_pos + 1;//or end_pos + 2

            if (tokens[pos].type == LBRA)
            {
                int end_pos;
                while_stat->push_child(parse_block(pos + 1, end_pos));
                pos = end_pos + 1;//end_pos - }
            } else
            {
                int end_pos;
                while_stat->push_child(parse_block(pos, end_pos, 1));
                //cout << "@@@[[[[[[" << s_val[tokens[end_pos].type] << "]]]]]]\n";
                pos = end_pos;
                if (tokens[pos].type == SEMICOLON)
                {
                    pos++;
                }
            }
        }//ifWHILE
        if (only_one_statement == 1)
        {
            endp = pos;
            return block;
        }
    }//while
    endp = pos;
    return block;
}



void read_tokens(ifstream &f, vector<Token> &lt)
{
    int cnt;
    f.read((char *)&cnt, sizeof(int));
    Token t;
    for (int i = 0; i < cnt; i++)
    {
        f.read((char *)&t, sizeof(Token));
        lt.push_back(t);
    }
    token_count = (int)lt.size();
}

void show_tokens(/*ofstream &f, */vector<Token> &lt)
{
    for (auto it = lt.begin(); it != lt.end(); it++)
    {
        cout << (int)(it - lt.begin()) << " [" << it->type << ":" << (it->type == NUM ? to_string(it->n_value) : it->s_value) << "]" << "\n";
    }
}


void show_ast(ASTnode *root, int level = 0)
{
    for (int i = 0; i < level; i++)
        cout << "    ";
    if (root->type == NUM)
    {
        cout << "NUM " << root->n_value << "\n";
    } else
    {
        cout << s_val[root->type];
        if (root->type == ID || root->type == FUNC_CALL || root->type == STRING)
        {
            cout << " " << root->s_value;
        }
        cout << "\n";
    }
    for (int i = 0; i < root->child_count; i++)
    {
        show_ast(root->childs[i], level + 1);
    }
}


int main()
{
    system("lexer.exe");
    ifstream in("tokens.bin", ios::binary);
    init_s_val();
    init_priorities();
    read_tokens(in, tokens);
    in.close();
    //show_tokens(tokens);

    int end_pos;
    show_ast(parse_block(0, end_pos));
    system("pause");
    return 0;
}
