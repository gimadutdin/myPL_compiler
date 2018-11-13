#include <bits/stdc++.h>
using namespace std;

//    lexer's constants(token types):
const int STRING = 0, NUM = 1, ID = 2, IF = 3, ELSE = 4, WHILE = 5, LBRA = 6, RBRA = 7, LPAR = 8,
RPAR = 9, PLUS = 10, MINUS = 11, MUL = 12, DIV = 13, MOD = 14, EQ = 15, NEQ = 16, LEQ = 17, GEQ = 18, LESS = 19,
GRT = 20, NOT = 21, AND = 22, OR = 23, ASSIGN = 24, SEMICOLON = 25, COMMA = 26, ONE_QUOTE = 27, EoF = 50;
//-------------------------------------------------------------------------
const int STRING_VALUE_SIZE = 256;
const int MAX_CHILD_1 = 30;
//--------------------------------------

//    checker's constants (states):
const int WAIT_OPND = 40/*wait operand*/, WAIT_OP = 41/*wait operation*/;
//const int WAIT_ALL = 42/*wait keyword or assignment*/, ;
//error types:
const int UNEXPECTED_TOKEN = 60, UNBALANCED_ROUND_BRACKETS = 61, UNBALANCED_CURLY_BRACKETS = 62,
EXPR_EXPECTED = 63, EXPR_STRING_EXPECTED = 64, LPAR_ASSIGN_EXPECTED = 65, UNEXPECTED_END_OF_FILE = 66, EXPECTED_SMTH = 67;

struct Token
{
    int type;
    int n_value;
    char s_value[STRING_VALUE_SIZE];
};
//--------------------------------
struct Error
{
    int type;
    int token_pos;
    int n_value;
};
//--------------------------------
vector<Token> tokens;
vector<Error> errors;
int tok_index, token_count;
char error_names[70][120];


void push_error(int type, int token_pos, int n_value = 0)
{
    Error e;
    e.type = type;
    e.token_pos = token_pos;
    e.n_value = n_value;
    errors.push_back(e);
}

void show_errors()
{
    for (int i = 0; i < (int)errors.size(); i++)
    {
        cout << "Error at token " << errors[i].token_pos << " : " << error_names[errors[i].type] << "\n";
    }
}

void init_error_names()
{
    strcpy(error_names[UNEXPECTED_TOKEN], "Unexpected token.");
    strcpy(error_names[UNBALANCED_ROUND_BRACKETS], "Unbalanced () brackets.");
    strcpy(error_names[UNBALANCED_CURLY_BRACKETS], "Unbalanced {} brackets.");
    strcpy(error_names[EXPR_EXPECTED], "Arithmetical/logic expression expected.");
    strcpy(error_names[EXPR_STRING_EXPECTED], "Arithmetical/logic expression or string expected.");
    strcpy(error_names[LPAR_ASSIGN_EXPECTED], "( or = expected.");
    strcpy(error_names[UNEXPECTED_END_OF_FILE], "Unexpected end of file.");
    strcpy(error_names[EXPECTED_SMTH], "Expected %s.");
}

bool isArLogOp(Token &t)
{
    return (t.type == PLUS || t.type == MINUS || t.type == MUL || t.type == DIV || t.type == MOD || t.type == NOT || t.type == OR || t.type == AND);
}

bool isCompOp(Token &t)
{
    return (t.type == EQ || t.type == NEQ || t.type == LESS || t.type == GRT || t.type == LEQ || t.type == GEQ);
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


bool check_expr_rec(int sp, int &end_pos, int &balance, int stop_pos, int stop_tok1/*type of stop token 1*/, int stop_tok2 = -1)
{
    int state = WAIT_OPND;//wait id, num or not (only one unary operator, unary minus already processed by lexer)

    Token cur_tok;
    while (tokens[sp].type != stop_tok1 && tokens[sp].type != stop_tok2 && sp != stop_pos)
    {
        cur_tok = tokens[sp];
        cout << "state = " << state << " sp = " << sp << " stop_pos = " << stop_pos << "\n";
        if (state == WAIT_OPND)
        {
            if (cur_tok.type == NUM || cur_tok.type == ID)
            {
                state = WAIT_OP;
                sp++;
            } else if (cur_tok.type == NOT)
            {
                //state remains the same
                sp++;
            } else if (cur_tok.type == LPAR)//opening bracket
            {
                balance++;
                //recursive self call:
                bool check_res = check_expr_rec(sp + 1, sp, balance, -1, stop_tok1, stop_tok2);
                if (!check_res)
                {
                    return false;
                }
                state = WAIT_OP;
            } else
            {
                push_error(UNEXPECTED_TOKEN, sp);
//                if (balance != 0)
//                {
//                    push_error(UNBALANCED_ROUND_BRACKETS, sp, balance);
//                }
                return false;
            }
        } else if (state == WAIT_OP)
        {
            if (isArLogOp(cur_tok) || isCompOp(cur_tok))
            {
                state = WAIT_OPND;
                sp++;
            } else if (cur_tok.type == RPAR)//closing bracket
            {
                if (balance == 0)
                {
                    push_error(UNBALANCED_ROUND_BRACKETS, sp, balance);
                    return false;
                }
                balance--;
                end_pos = sp + 1;
                return true;
            } else
            {
                push_error(UNEXPECTED_TOKEN, sp);
//                if (balance != 0)
//                {
//                    push_error(UNBALANCED_ROUND_BRACKETS, sp, balance);
//                }
                return false;
            }
        }
    }//while
    end_pos = sp;//stop_tok if all normal
    //cout << "((((((" << sp << ")))))";
    return (balance == 0);//!!!!!!!
}

bool check_expr(int sp, int &end_pos, int bal, int stop_pos, int stop_tok1, int stop_tok2 = -1)//wrapper for check_expr_rec
{
    int balance = bal;
    bool res = check_expr_rec(sp, end_pos, balance, stop_pos, stop_tok1, stop_tok2);//end_pos and balance is sent by link
    return res;
}


bool check_block(int sp, int &end_pos, int only_one_statement = 0)
{//cout << "SPP = " << sp << " and ts = " << tokens.size() << "\n";
    if (tokens[sp].type == EoF)
    {
        push_error(UNEXPECTED_END_OF_FILE, sp);
        return false;
    }
    int was_rpar = 0;
    while (sp < tokens.size() - 1 && tokens[sp].type != RBRA)
    {
        cout << "SP = " << sp << "\n";
        if (tokens[sp].type == ID)//if there if ID, so two cases - function call or assignment a value to id
        {
        	sp++;
            if (tokens[sp].type == EoF)
            {
                push_error(UNEXPECTED_END_OF_FILE, sp);
                return false;
            }
            if (tokens[sp].type == LPAR)////=========================================================================processing ID(arg0, ..., argn)
            {
            	sp++;//now sp is number of first token after (
                int i = sp;
                while (tokens[i].type != SEMICOLON && tokens[i].type != EoF)
                {
                    i++;
                }
                if (tokens[i - 1].type == EoF)
                {
                    push_error(UNEXPECTED_END_OF_FILE, i);
                    return false;
                }
                if (tokens[i - 1].type != RPAR)
                {
                    push_error(EXPECTED_SMTH, i - 1, RPAR);
                    return false;
                }
                int stop_pos = i - 1;//number of ) char which is before ;
                while (sp < stop_pos)
                {
                    if (tokens[sp].type == COMMA)
                    {
                        sp++;
                    } else if (tokens[sp].type == STRING)
                    {
                        sp++;
                    } else//expr
                    {
                        int endp;
                        bool expr_check_res = check_expr(sp, endp, 0, stop_pos, COMMA);
                        if (!expr_check_res)
                        {
                            return false;
                        }
                        if (endp == stop_pos)
                        {
                            sp = stop_pos + 2;
                            break;
                        }
                        sp = endp + 1;
                    }
                }//while sp != stop_pos
            } else if (tokens[sp].type == ASSIGN)////=========================================================================processing ID = ...
            {
                if (tokens[sp + 1].type == EoF)
                {
                    push_error(UNEXPECTED_END_OF_FILE, sp + 1);
                    return false;
                }
                if (tokens[sp + 1].type == SEMICOLON)
                {
                    push_error(EXPR_EXPECTED, sp + 1);
                    return false;
                }
                int endp;
                bool expr_check_res = check_expr(sp + 1, endp, 0, -1, SEMICOLON);
                if (!expr_check_res)
                {
                    return false;
                }
                sp = endp + 1;//now sp is number of char after ;
            } else
            {
                push_error(LPAR_ASSIGN_EXPECTED, sp);
                return false;
            }
        } else if (tokens[sp].type == IF)//=========================================================================processing IF ... ELSE
        {
            if (tokens[sp + 1].type == LPAR)
            {
                sp++;
            } else
            {
                push_error(EXPECTED_SMTH, sp, LPAR);
                return false;
            }
            int i = sp, bal = 0;
            do
            {
                if (tokens[i].type == LPAR)
                {
                    bal++;
                }
                if (tokens[i].type == RPAR)
                {
                    bal--;
                }
                if (bal != 0 && tokens[i].type == EOF)
                {
                	push_error(UNEXPECTED_END_OF_FILE, i);
                	return false;
                }
                i++;
            } while (bal != 0);
            int stop_pos = i - 1;//number of ) token before first token of IF's body
            int endp;
            bool expr_check_res = check_expr(sp + 1, endp, 0, stop_pos, SEMICOLON);
            if (!expr_check_res)
            {
                return false;
            }
            sp = endp + 1;//now sp is number of first token of IF's body
            //processing IF's body
            bool res;
            if (tokens[sp].type == LBRA)//if there is a block
            {cout << "[[[[[[[" << sp << "]]]]]]]";
                res = check_block(sp + 1, endp);//recursive call to process {} block
                cout << boolalpha << res << "\n";
                if (!res) return false;
                sp = endp + 1;
            } else//if there are a single statement
            {
                res = check_block(sp, endp, 1);//recursive call to process only 1 statement
                if (!res) return false;
                sp = endp + 1;
            }
            //sp is number of token after IF's body
            if (tokens[sp].type == ELSE)//if there is ELSE branch, we process its body
            {
                sp++;
                if (tokens[sp].type == LBRA)
                {
                    check_block(sp + 1, endp);//recursive self call to process {} block
                    sp = endp + 1;
                } else
                {
                    check_block(sp + 1, endp, 1);//recursive self call to process only 1 statement
                    sp = endp + 1;
                }
            }
            //now sp is number of first token after ELSE's body or if there is not ELSE branch sp is number of token after IF's body
        } else if (tokens[sp].type == WHILE)//=========================================================================processing WHILE
        {
            if (tokens[sp + 1].type == LPAR)
            {
                sp++;
            } else
            {
                push_error(EXPECTED_SMTH, sp, LPAR);
                return false;
            }
            int i = sp, bal = 0;
            do
            {
                if (tokens[i].type == LPAR)
                {
                    bal++;
                }
                if (tokens[i].type == RPAR)
                {
                    bal--;
                }
                if (bal != 0 && tokens[i].type == EoF)
                {
                	push_error(UNEXPECTED_END_OF_FILE, i);
                	return false;
                }
                i++;
            } while (bal != 0);
            int stop_pos = i - 1;//number of ) token before first token of WHILE's body
            int endp;
            bool expr_check_res = check_expr(sp + 1, endp, 0, stop_pos, SEMICOLON);
            if (!expr_check_res)
            {
                return false;
            }
            sp = endp + 1;//now sp is number of first token of WHILE's body
            //processing WHILE's body
            bool res;
            if (tokens[sp].type == LBRA)//if there is a block
            {
                res = check_block(sp + 1, endp);//recursive call to process {} block
                if (!res) return false;
                //cout << "{{{{{{{" << endp << "}}}}}}}}}";
                sp = endp + 1;
            } else//if there are a single statement
            {
                res = check_block(sp, endp, 1);//recursive call to process only 1 statement
                if (!res) return false;
                sp = endp + 1;
            }
            //sp is number of token after WHILE's body
        } else if (tokens[sp].type == RPAR)
        {
            end_pos = sp;
            return true;
        }
//        else if (tokens[sp].type == EoF)
//        {
//            push_error(UNEXPECTED_END_OF_FILE, sp);
//            end_pos = sp;
//            return false;
//        }
    	if (only_one_statement == 1)
        {
            end_pos = sp;
            return true;
        }
        end_pos = sp;
    }
    end_pos = sp;
    push_error(UNBALANCED_CURLY_BRACKETS, sp);//check
    return false;//we are there because we havent meet } and only_one_statement wasnt 1
}



int main()
{
    init_error_names();

    ifstream in("tokens.bin", ios::binary);
    read_tokens(in, tokens);
    in.close();
    Token t;
    t.type = EoF;
    tokens.push_back(t);
    show_tokens(tokens);

    int endp, bal, stop_pos = (int)tokens.size();
    cout << boolalpha << check_block(0, endp) << "\n";
    //cout << boolalpha << check_expr(0, endp, 0, stop_pos, SEMICOLON) << "\n";
    show_errors();
    return 0;
}
