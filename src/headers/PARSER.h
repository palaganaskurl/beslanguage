#ifndef _PARSER_H
#define _PARSER_H

#include "..\headers\LINKEDLIST.H"

void initToken(TokenList *); // initialize token to be global 
void readToken(); // read token by token
int match(TokenTypeBes); // check if token match the syntax
void expectDeclaration(); // expect declaration
void expectAssignment(); // expect assignment
void displayError(int, char *); // error message
void errorRecovery(); // error recovery
void expectCondition(); // for the condition statements
void expectLoop(); // for the looping statements 
void expectConditionals(); // for the conditional statements
void expectUnaryOperation(); // for the unary operations
void elseIf(); // for the else if statement
void expectInput(); // for the input statements
void getNextToken(); // get next token
void expectWrite(); // for the output statement
void expectConcat(); // for recursing the concat
void expectArithmetic(); // for arithmetic expressions

char * getVarName();
void checkIfVariableDeclared(char *);
void statementsNatural();
void depthFunction();

extern char errorList[1000][1000];
#endif


