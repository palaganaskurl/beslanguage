#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\headers\LINKEDLIST.H"
#include "..\headers\SYMBOLTABLE.H"

HashTable * hashTableCreate(int size){
	HashTable * hashTable = NULL;
	int i;
	if(size < 1) return NULL;
	if((hashTable = malloc(sizeof(HashTable))) == NULL)
		return NULL;
	if((hashTable->table = malloc(sizeof(HashTableNode *) * size)) == NULL)
		return NULL;
	for(i = 0; i < size; i++)
		hashTable->table[i] = NULL;
	hashTable->size = size;
	return hashTable;
}

int hash(HashTable * hashTable, char * variableName){
	unsigned long int hashVal;
	int i = 0;
	while(hashVal < ULONG_MAX && i < strlen(variableName)){
		hashVal = hashVal << 8;
		hashVal += variableName[i];
		i++;
	}
	return hashVal % hashTable->size;
}

HashTableNode * hashTableNewValues(char * variableName, TokenTypeBes tokenType,  char * variableValue){
	HashTableNode * newValues;
	if((newValues = malloc(sizeof(HashTableNode))) == NULL)
		return NULL;
	if((newValues->variableName = strdup(variableName)) == NULL)
		return NULL;
	if((newValues->variableValue = strdup(variableValue)) == NULL)
		return NULL;
	newValues->tokenType = tokenType;
	newValues->isUsed = 0;
	newValues->next = NULL;
	return newValues;
}

void hashTableSet(HashTable * hashTable,  char * variableName, TokenTypeBes tokenType, char * variableValue){
	int bin = 0;
	HashTableNode * newValues = NULL;
	HashTableNode * next = NULL;
	HashTableNode * last = NULL;
	
	bin = hash(hashTable, variableName);
	
	next = hashTable->table[bin];
	
	while(next != NULL && next->variableName != NULL && strcmp(variableName, next->variableName) > 0){
		last = next;
		next = next->next;
	}
	
	if(next != NULL && next->variableName != NULL && strcmp(variableName, next->variableName) == 0){
		free(next->variableValue);
		next->variableValue = strdup(variableValue);
	} else {
		newValues = hashTableNewValues(variableName, tokenType, variableValue);
		if(next == hashTable->table[bin]){
			newValues->next = next;
			hashTable->table[bin] = newValues;
		} else if(next == NULL){
			last->next = newValues;
		} else {
			newValues->next = next;
			last->next = newValues;
		}
	}
}

void hashTableSetIsUsed(HashTable * hashTable, char * variableName){
	int bin = 0;
	HashTableNode * values;
	
	bin = hash(hashTable, variableName);
	
	values = hashTable->table[bin];
	while(values != NULL && values->variableName != NULL && strcmp(variableName, values->variableName) > 0)
		values = values->next;
	
	if(values == NULL || values->variableName == NULL || strcmp(variableName, values->variableName) != 0)
		return;
	else
		values->isUsed = 1;
}

char * hashTableGetValue(HashTable * hashTable, char * variableName){
	int bin = 0;
	HashTableNode * values;
	
	bin = hash(hashTable, variableName);
	
	values = hashTable->table[bin];
	while(values != NULL && values->variableName != NULL && strcmp(variableName, values->variableName) > 0)
		values = values->next;
	
	if(values == NULL || values->variableName == NULL || strcmp(variableName, values->variableName) != 0)
		return NULL;
	else
		return values->variableValue;
}

TokenTypeBes hashTableGetTokenType(HashTable * hashTable, char * variableName){
	int bin = 0;
	HashTableNode * values;
	
	bin = hash(hashTable, variableName);
	
	values = hashTable->table[bin];
	while(values != NULL && values->variableName != NULL && strcmp(variableName, values->variableName) > 0)
		values = values->next;
	
	if(values == NULL || values->variableName == NULL || strcmp(variableName, values->variableName) != 0)
		return -1;
	else
		return values->tokenType;
}

int hashTableGetIsUsed(HashTable * hashTable, char * variableName){
	int bin = 0;
	HashTableNode * values;
	
	bin = hash(hashTable, variableName);
	
	values = hashTable->table[bin];
	while(values != NULL && values->variableName != NULL && strcmp(variableName, values->variableName) > 0)
		values = values->next;
	
	if(values == NULL || values->variableName == NULL || strcmp(variableName, values->variableName) != 0)
		return -1;
	else
		return values->isUsed;
}

ScopeHashTable * scopeHashTableCreate(int size){
	ScopeHashTable * scopeHashTable = NULL;
	int i;
	if(size < 1) return NULL;
	if((scopeHashTable = malloc(sizeof(ScopeHashTable))) == NULL)
		return NULL;
	if((scopeHashTable->table = malloc(sizeof(HashTableScopeNode *) * size)) == NULL)
		return NULL;
	for(i = 0; i < size; i++)
		scopeHashTable->table[i] = NULL;
	scopeHashTable->size = size;
	return scopeHashTable;
}

int scopeHash(ScopeHashTable * scopeHashTable, char * variableName){
	unsigned long int hashVal;
	int i = 0;
	while(hashVal < ULONG_MAX && i < strlen(variableName)){
		hashVal = hashVal << 8;
		hashVal += variableName[i];
		i++;
	}
	return hashVal % scopeHashTable->size;
}

HashTableScopeNode * scopeHashTableNewValues(char * variableName, TokenTypeBes tokenType,  char * variableValue, int depth){
	HashTableScopeNode * newValues;
	if((newValues = malloc(sizeof(HashTableScopeNode))) == NULL)
		return NULL;
	if((newValues->variableName = strdup(variableName)) == NULL)
		return NULL;
	if((newValues->variableValue = strdup(variableValue)) == NULL)
		return NULL;
	newValues->tokenType = tokenType;
	newValues->depth = depth;
	newValues->isUsed = 0;
	newValues->next = NULL;
	return newValues;
}

void scopeHashTableSet(ScopeHashTable * scopeHashTable,  char * variableName, TokenTypeBes tokenType, char * variableValue, int depth){
	int bin = 0;
	HashTableScopeNode * newValues = NULL;
	HashTableScopeNode * next = NULL;
	HashTableScopeNode * last = NULL;
	
	bin = scopeHash(scopeHashTable, variableName);
	
	next = scopeHashTable->table[bin];
	
	while(next != NULL && next->variableName != NULL && strcmp(variableName, next->variableName) > 0){
		last = next;
		next = next->next;
	}
	
	if(next != NULL && next->variableName != NULL && strcmp(variableName, next->variableName) == 0){
		free(next->variableValue);
		next->variableValue = strdup(variableValue);
	} else {
		newValues = scopeHashTableNewValues(variableName, tokenType, variableValue, depth);
		if(next == scopeHashTable->table[bin]){
			newValues->next = next;
			scopeHashTable->table[bin] = newValues;
		} else if(next == NULL){
			last->next = newValues;
		} else {
			newValues->next = next;
			last->next = newValues;
		}
	}
}

void scopeHashTableSetIsUsed(ScopeHashTable * scopeHashTable, char * variableName){
	int bin = 0;
	HashTableScopeNode * values;
	
	bin = scopeHash(scopeHashTable, variableName);
	
	values = scopeHashTable->table[bin];
	while(values != NULL && values->variableName != NULL && strcmp(variableName, values->variableName) > 0)
		values = values->next;
	
	if(values == NULL || values->variableName == NULL || strcmp(variableName, values->variableName) != 0)
		return;
	else
		values->isUsed = 1;
}

char * scopeHashTableGetValue(ScopeHashTable * scopeHashTable, char * variableName){
	int bin = 0;
	HashTableScopeNode * values;
	
	bin = scopeHash(scopeHashTable, variableName);
	
	values = scopeHashTable->table[bin];
	while(values != NULL && values->variableName != NULL && strcmp(variableName, values->variableName) > 0)
		values = values->next;
	
	if(values == NULL || values->variableName == NULL || strcmp(variableName, values->variableName) != 0)
		return NULL;
	else
		return values->variableValue;
}

TokenTypeBes scopeHashTableGetTokenType(ScopeHashTable * scopeHashTable, char * variableName){
	int bin = 0;
	HashTableScopeNode * values;
	
	bin = scopeHash(scopeHashTable, variableName);
	
	values = scopeHashTable->table[bin];
	while(values != NULL && values->variableName != NULL && strcmp(variableName, values->variableName) > 0)
		values = values->next;
	
	if(values == NULL || values->variableName == NULL || strcmp(variableName, values->variableName) != 0)
		return -1;
	else
		return values->tokenType;
}

int scopeHashTableGetDepth(ScopeHashTable * scopeHashTable, char * variableName){
	int bin = 0;
	HashTableScopeNode * values;
	
	bin = scopeHash(scopeHashTable, variableName);
	
	values = scopeHashTable->table[bin];
	while(values != NULL && values->variableName != NULL && strcmp(variableName, values->variableName) > 0)
		values = values->next;
	
	if(values == NULL || values->variableName == NULL || strcmp(variableName, values->variableName) != 0)
		return -1;
	else
		return values->depth;
}

void scopeHashTableRemoveVariable(ScopeHashTable * scopeHashTable, char * variableName, int depth){
	int bin = 0;
	HashTableScopeNode * values;
	
	bin = scopeHash(scopeHashTable, variableName);
	
	values = scopeHashTable->table[bin];
	while(values != NULL){
		if(strcmp(variableName, values->variableName) == 0 && depth == values->depth){
//			printf("DEPTH : %d\n", depth);
//			printf("VAL DEPTH : %d\n", values->depth);
//			printf("VAR : %s", variableName);
//			printf("VAL VAR : %s", values->variableName);
			values->variableName = "";
			values->variableValue = "";
			values->tokenType = -1;
			values->depth = -1;
		}
		values = values->next;
	}
}
