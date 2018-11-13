#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

//    lexer's constants(token types):
const int STRING = 0, NUM = 1, ID = 2, IF = 3, ELSE = 4, WHILE = 5, LBRA = 6, RBRA = 7, LPAR = 8,
RPAR = 9, PLUS = 10, MINUS = 11, MUL = 12, DIV = 13, MOD = 14, EQ = 15, NEQ = 16, LEQ = 17, GEQ = 18, LESS = 19,
GRT = 20, NOT = 21, AND = 22, OR = 23, ASSIGN = 24, SEMICOLON = 25, COMMA = 26, ONE_QUOTE = 27;
//    Additional constants (AST node types):
const int BLOCK = 40, FUNC_CALL = 41;
//-------------------------------------------------------------------------

const int STRING_VALUE_SIZE = 256;
const int MAX_CHILD_1 = 30;
//-------------------------------------------------------------------------
// Generator's constants : command types
const int CMD_PUSH = 5, CMD_POP = 6, CMD_PUSHS = 7, CMD_FETCH = 8, CMD_STORE = 9,
CMD_CALL = 10, CMD_JMP = 11, CMD_JZ = 12, CMD_JNZ = 13, CMD_PLUS = 14, CMD_MINUS = 15,
CMD_MUL = 16, CMD_DIV = 17, CMD_MOD = 18, CMD_AND = 19, CMD_OR = 20, CMD_NOT = 21,
CMD_EQ = 22, CMD_NEQ = 23, CMD_LESS = 24, CMD_GRT = 25, CMD_LEQ = 26, CMD_GEQ = 27;


#endif // CONSTANTS_H_INCLUDED
