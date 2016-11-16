#ifndef _SYMBOLTABLE_H
#define _SYMBOLTABLE_H

#include "..\headers\LINKEDLIST.H"

typedef struct TableNode{
	char * variableName;
	TokenTypeBes tokenType;
	char * variableValue;
	int isUsed;
	struct TableNode * next;
} HashTableNode;

typedef struct ScopeTableNode{
	char * variableName;
	TokenTypeBes tokenType;
	char * variableValue;
	int depth;
	int isUsed;
	struct ScopeTableNode * next;
} HashTableScopeNode;

typedef struct Table{
	int size;
	struct TableNode ** table;
} HashTable;

typedef struct ScopeTable{
	int size;
	struct ScopeTableNode ** table;
} ScopeHashTable;

HashTable * hashTableCreate(int);
int hash(HashTable *, char *);
HashTableNode * hashTableNewValues(char *, TokenTypeBes, char *);
void hashTableSet(HashTable *, char *, TokenTypeBes, char *);
void hashTableSetIsUsed(HashTable *, char *);
char * hashTableGetValue(HashTable *, char *);
TokenTypeBes hashTableGetTokenType(HashTable *, char *);
int hashTableGetIsUsed(HashTable *, char *);

ScopeHashTable * scopeHashTableCreate(int);
int scopeHash(ScopeHashTable *, char *);
HashTableScopeNode * scopeHashTableNewValues(char *, TokenTypeBes, char *, int);
void scopeHashTableSet(ScopeHashTable *, char *, TokenTypeBes, char *, int);
void scopeHashTableSetIsUed(ScopeHashTable *, char *);
char * scopeHashTableGetValue(ScopeHashTable *, char *);
TokenTypeBes scopeHashTableGetTokenType(ScopeHashTable *, char *);
int scopeHashTableGetDepth(ScopeHashTable *, char *);
void scopeHashTableRemoveVariable(ScopeHashTable *, char *, int);

#endif
