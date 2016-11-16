#ifndef _CODE_GENERATION_H
#define _CODE_GENERATION_H

#include "..\headers\SYMBOLTABLE.H"
#include "..\headers\LINKEDLIST.H"


void generateCode(TokenList *);
void initHashTable(HashTable *, ScopeHashTable *);

#endif
