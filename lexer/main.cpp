/*
    Lexer for myPL
    Author: Rustem Gimadutdinov
*/

#include <bits/stdc++.h>
using namespace std;

const int STRING = 0, NUM = 1, ID = 2, IF = 3, ELSE = 4, WHILE = 5, LBRA = 6, RBRA = 7, LPAR = 8,
RPAR = 9, PLUS = 10, MINUS = 11, MUL = 12, DIV = 13, MOD = 14, EQ = 15, NEQ = 16, LEQ = 17, GEQ = 18, LESS = 19,
GRT = 20, NOT = 21, AND = 22, OR = 23, ASSIGN = 24, SEMICOLON = 25, COMMA = 26, ONE_QUOTE = 27;

char s_val[50][64];

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
}

struct Token
{
    int type, n_value;
    char s_value[256];
};

int gpos;//global position in input file
int flen;//length of file
char *text;//source code from file
vector<Token> token_list;//list of tokens

//write a function which will write token_list to output file
//write function next_token which will return next token
//struct el-t or error (its code will be written to a parameter sent by link)

char* readFile(char fileName[]) {
    ifstream ifs(fileName);
    if(ifs.is_open())
    {
        string s;
        s.assign((istreambuf_iterator<char>(ifs.rdbuf())), istreambuf_iterator<char>());
        char *res = new char[s.length() + 5];
        strcpy(res, s.c_str());
        return res;
        ifs.close();
    } else
    {
        cout << "Unable to open file" << std::endl;
        return NULL;
    }
}

bool is_digit(char x) {return x >= '0' && x <= '9';}
bool is_small_letter(char x) {return x >= 'a' && x <= 'z';}
bool is_big_letter(char x) {return x >= 'A' && x <= 'Z';}
bool is_underlin_char(char x) {return x == '_';}


int read_num(int sp, int &ans)
{
    int num = 0;
    while (sp < flen && is_digit(text[sp]))
    {
        num = (num + (text[sp] - '0'))*10;
        sp++;
    }
    ans = num/10;
    return sp;
}

int read_string(int sp, char str[])
{
    int i = 0;
    while (sp < flen && text[sp] != '\n' && text[sp] != s_val[ONE_QUOTE][0])
    {
        str[i] = text[sp];
        i++;
        sp++;
    }
    str[i] = '\0';
    if (text[sp] == s_val[ONE_QUOTE][0])
    {
        return sp + 1;
    }
    return -1;
}


int read_id(int sp, char id[])
{
    int i = 0;
    while (sp < flen && (is_small_letter(text[sp]) || is_big_letter(text[sp]) || is_digit(text[sp]) || text[sp] == '_'))
    {
        id[i] = text[sp];
        i++;
        sp++;
    }
    id[i] = '\0';
    return sp;
}


void show_token_list(ofstream &f, vector<Token> &lt)
{
    for (auto it = lt.begin(); it != lt.end(); it++)
    {
        f << (int)(it - lt.begin()) << " [" << it->type << ":" << (it->type == NUM ? to_string(it->n_value) : it->s_value) << "]" << "\n";
    }
}

void write_token_list_bin(ofstream &f, vector<Token> &lt)
{
    int cnt = (int)lt.size();
    f.write((char *)&cnt, sizeof(int));
    for (auto it = lt.begin(); it != lt.end(); it++)
    {
        f.write((char *)&(*it), sizeof(Token));
    }
}


int search_token(int sp, int tok)//tok - type of token to search, sp - start position
//if token was found function returns end position else returns -1
{

    int t_end = sp + strlen(s_val[tok]);
    if (t_end > strlen(text))
        return -1;

    bool match = true;
    for (int i = sp; i < t_end && match; i++)
    {
        if (text[i] != s_val[tok][i - sp])
            match = false;
    }
    return (match ? t_end : -1);
}



Token next_token(int &result)//uses text
{Token a;
    while (text[gpos] == ' ' || text[gpos] == '\n')
    {
        gpos++;
    }
    //searching keywords and special chars
    for (int tok_type = 3; tok_type <= 26; tok_type++)
    {
        int search_res = search_token(gpos, tok_type);
        if (search_res != -1)
        {
            Token t;
            t.type = tok_type;
            strcpy(t.s_value, s_val[tok_type]);
            result = 1;//good, next token found
            gpos = search_res;//if token was found search_res is the end of its entry in text
            return t;
        }
    }
    //search identifiers
    char c = text[gpos];
    if (is_small_letter(c) || is_big_letter(c) || c == '_')
    {
        char id_value[256];
        int search_res = read_id(gpos, id_value);
        Token t;
        t.type = ID;
        strcpy(t.s_value, id_value);
        result = 1;
        gpos = search_res;
        return t;
    }
    //get string
    if (c == s_val[ONE_QUOTE][0])
    {
        char str[256];
        gpos++;
        int search_res = read_string(gpos, str);
        if (search_res != -1)
        {
            Token t;
            t.type = STRING;
            strcpy(t.s_value, str);
            result = 1;
            gpos = search_res;
            return t;
        } else
        {
            result = 0;
            cout << "Error, expected end of string - quote symbool not found.";
            return a;
        }
    }

    if (is_digit(c))
    {
        int val;
        int search_res = read_num(gpos, val);
        Token t;
        t.type = NUM;
        t.n_value = val;
        if (!token_list.empty() && token_list.back().type == MINUS)//is minus is unary
        {
            if (token_list.size() > 1)
            {
                Token lbo = *(token_list.end() - 2);
                if (lbo.type != ID && lbo.type != NUM && lbo.type != RPAR)
                {
                    t.n_value *= -1;
                    token_list.pop_back();
                }
            }
        }
        result = 1;
        gpos = search_res;
        return t;
    }

    result = 0;Token t; return t;
}

int main()
{
    init_s_val();
    text = readFile("source.txt");
    gpos = 0;
    flen = strlen(text);

    int res;
    do
    {
        Token t;
        t = next_token(res);
        if (res)
        {
            token_list.push_back(t);
        }
    } while (res);

    ofstream out("token_list.txt");
    show_token_list(out, token_list);
    out.close();

    ofstream bin_out("tokens.bin", ios::binary);
    write_token_list_bin(bin_out, token_list);
    bin_out.close();

    return 0;
}
