#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "..\headers\LINKEDLIST.H"

Stack * stack_create(){
    Stack * stack = malloc(sizeof(Stack));
    assert(stack != NULL);

    stack->tokenType = START;
    stack->next = NULL;

    return stack;
}

TokenList * list_create(){
    TokenList * tokenList = malloc(sizeof(TokenList));
    assert(tokenList != NULL);

    Stack * stack = stack_create();
    tokenList->first = stack;
    
    return tokenList;
}

void listAppend(TokenList * tokenList, TokenTypeBes tokenType, int lineNumber){
	assert(tokenList != NULL);
	
	Stack * stack = tokenList->first;
	while(stack->next != NULL)
		stack = stack->next;
		
	stack->tokenType = tokenType;
	stack->tokenLine = lineNumber;
	stack->next = stack_create();
}
