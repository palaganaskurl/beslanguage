#ifndef _LINKEDLIST_H
#define _LINKEDLIST_H

typedef enum{
	ASS_OPER, // 0
	REL_OPER, // 1
	ARITH_OPER, // 2
	LOG_OPER, // 3
	UNA_OPER, // 4
	STR_LIT, // 5
	CHR_LIT, // 6 
	ESC_SEQ, // 7
	DECIMAL_LIT, // 8
	NUMBER_LIT, // 9
	OPEN_PAREN, // 10
	CLOSE_PAREN, // 11
	OPEN_BRACE, // 12
	CLOSE_BRACE, // 13
	OPEN_BRACKET, // 14
	CLOSE_BRACKET,// 15
	SEMICOLON, // 16
	BREAK_KW, // 17
	BOOL_KW, // 18
	CASE_KW, // 19
	CHAR_KW, // 20
	CONTINUE_KW, // 21
	DEFAULT_KW, // 22
	DO_KW, // 23
	DOUBLE_KW, // 24
	ELSE_KW, // 25
	ENUM_KW, // 26 
	FINAL_KW, // 27
	FLOAT_KW, // 28
	FOR_KW, // 29
	FALSE_RW, // 30
	IF_KW, // 31
	INT_KW, // 32 
	IMPORT_KW, // 33
	NULL_RW, // 34
	RETURN_KW, // 35
	STATIC_KW, // 36 
	STRING_KW, // 37
	SWITCH_KW, // 38 
	TRUE_RW, // 39
	VOID_KW, // 40
	WHILE_KW, // 41
	VALID_IDEN, // 42
	COMMA, // 43
	READ_FN, // 44
	WRITE_FN, // 45
	WRITELN_FN, // 46
	CONCAT_OPER, // 47
	ASS_COMP_OPER, // 48
	START // 49
} TokenTypeBes;

typedef struct Node{
	TokenTypeBes tokenType;
	int tokenLine;
	char lexeme[1000];
	struct Node * next;
} Stack;

typedef struct List{
    struct Node * first;
} TokenList;

Stack * stack_create(); // create stack
TokenList * list_create(); // create the token list

// stack functions
void listAppend(TokenList *, TokenTypeBes, int);

#endif
