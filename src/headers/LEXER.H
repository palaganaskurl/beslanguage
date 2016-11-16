#ifndef _LEXER_H
#define _LEXER_H

#include "..\headers\LINKEDLIST.H"

void initLexer(FILE *, FILE *fo);

void isOperator(char, FILE *, FILE *); // output for symbol table operators
void isDelimiter(char, FILE *, FILE *); // output for symbol table delimiters
void isNumberLiteral(char, FILE *, FILE *); // output for symbol table number literal
void isStringLiteral(char, FILE *, FILE *); // output for symbol table string literal
void isCharacterLiteral(char, FILE *, FILE *); // output for symbol table character literal
void isComment(char, FILE *, FILE *); // output for symbol table comment
char isEscapeSequence(char, FILE *, FILE *); // output for symbol table escape sequence
void isKeyword(char, FILE *, FILE *); // output for symbol table keyword

int isOper(char); // check if operator
int isDelim(char); // check if delimiter

void checkNewLine(char);

char getNextCharacter(FILE *, FILE *, char);
void fileOutput(FILE *, const char *);
void fileOutputX(FILE *, const char *, char);
void outputSpace(FILE *);

void getLexemes(FILE *);
const char * getfield(char *, int);

TokenList * createTokenList(); // create token list
#endif
