#include <stdio.h>
#include <conio.h>
#include <string.h>

#include "..\headers\LINKEDLIST.H"
#include "..\headers\SYMBOLTABLE.H"

HashTable * gHashTable;
ScopeHashTable * gScopeHashTable;

void initHashTable(HashTable * hashTable, ScopeHashTable * scopeHashTable){
	gHashTable = hashTable;
	gScopeHashTable = scopeHashTable;
}

void generateCode(TokenList * tokenList){
	Stack * stack = tokenList->first;
	FILE * fp = fopen("gencode.c", "w");
	char * lastVar;
	size_t pos;
//	size_t pos = ftell(fp);    // Current position
//	fseek(fp, 0, SEEK_END);    // Go to end
//	size_t length = ftell(fp); // read the position which is the size
//	fseek(fp, pos, SEEK_SET);  // restore original posistion
	fprintf(fp, "#include <stdio.h>\n");
	fprintf(fp, "#include <conio.h>\n");
	fprintf(fp, "int main(){");
	while(stack->next != NULL){
		switch(stack->tokenType){
			case WRITE_FN:{
				char arrVar[1000][1000];
				int counter = 0;
				fprintf(fp, "printf(\"");
				stack = stack->next;
				stack = stack->next;
				while(stack->tokenType != CLOSE_PAREN){
					if(stack->tokenType == STR_LIT){
						fprintf(fp, stack->lexeme);
					} else if(stack->tokenType == VALID_IDEN){
						if(scopeHashTableGetTokenType(gScopeHashTable, stack->lexeme) != -1){
							if(scopeHashTableGetTokenType(gScopeHashTable, stack->lexeme) == STRING_KW){
								fprintf(fp, "%%s");
								strcpy(arrVar[counter], stack->lexeme);
								counter++;
							} else if(scopeHashTableGetTokenType(gScopeHashTable, stack->lexeme) == INT_KW){
								fprintf(fp, "%%d");
								strcpy(arrVar[counter], stack->lexeme);
								counter++;
							} else if(scopeHashTableGetTokenType(gScopeHashTable, stack->lexeme) == FLOAT_KW ||
									scopeHashTableGetTokenType(gScopeHashTable, stack->lexeme) == DOUBLE_KW){
								fprintf(fp, "%%f");
								strcpy(arrVar[counter], stack->lexeme);
								counter++;  	
							}
						} else if(hashTableGetTokenType(gHashTable, stack->lexeme) != -1){
							if(hashTableGetTokenType(gHashTable, stack->lexeme) == STRING_KW){
								fprintf(fp, "%%s");
								strcpy(arrVar[counter], stack->lexeme);
								counter++;
							} else if(hashTableGetTokenType(gHashTable, stack->lexeme) == INT_KW){
								fprintf(fp, "%%d");
								strcpy(arrVar[counter], stack->lexeme);
								counter++;
							} else if(hashTableGetTokenType(gHashTable, stack->lexeme) == FLOAT_KW ||
									hashTableGetTokenType(gHashTable, stack->lexeme) == DOUBLE_KW){
								fprintf(fp, "%%f");
								strcpy(arrVar[counter], stack->lexeme);			
								counter++;
							}
						} else {
							fprintf(fp, "%%d");
							strcpy(arrVar[counter], stack->lexeme);
							counter++;
						}
					}
					stack = stack->next;
				}
				fprintf(fp, "\"");
				if(counter != 0){
					int i;
					fprintf(fp, ",");
					for(i = 0; i < counter; i++){
						fprintf(fp, arrVar[i]);
						if(i == counter - 1)
							break;
						fprintf(fp, ",");
					}
				}
				fprintf(fp, ")");
				break;
			}
			case WRITELN_FN:{
				char arrVar[1000][1000];
				int counter = 0;
				fprintf(fp, "printf(\"");
				stack = stack->next;
				stack = stack->next;
				while(stack->tokenType != CLOSE_PAREN){
					if(stack->tokenType == STR_LIT){
						fprintf(fp, stack->lexeme);
					} else if(stack->tokenType == VALID_IDEN){
						if(scopeHashTableGetTokenType(gScopeHashTable, stack->lexeme) != -1){
							if(scopeHashTableGetTokenType(gScopeHashTable, stack->lexeme) == STRING_KW){
								fprintf(fp, "%%s");
								strcpy(arrVar[counter], stack->lexeme);
								counter++;
							} else if(scopeHashTableGetTokenType(gScopeHashTable, stack->lexeme) == INT_KW){
								fprintf(fp, "%%d");
								strcpy(arrVar[counter], stack->lexeme);
								counter++;
							}
						} else if(hashTableGetTokenType(gHashTable, stack->lexeme) != -1){
							if(hashTableGetTokenType(gHashTable, stack->lexeme) == STRING_KW){
								fprintf(fp, "%%s");
								strcpy(arrVar[counter], stack->lexeme);
								counter++;
							} else if(hashTableGetTokenType(gHashTable, stack->lexeme) == INT_KW){
								fprintf(fp, "%%d");
								strcpy(arrVar[counter], stack->lexeme);
								counter++;
							}
						} else {
							fprintf(fp, "%%d");
							strcpy(arrVar[counter], stack->lexeme);
							counter++;
						}
					}
					stack = stack->next;
				}
				fprintf(fp, "\\n\"");
				if(counter != 0){
					int i;
					fprintf(fp, ",");
					for(i = 0; i < counter; i++){
						fprintf(fp, arrVar[i]);
						if(i == counter - 1)
							break;
						fprintf(fp, ",");
					}
				}
				fprintf(fp, ")");
				break;
			}
			case VALID_IDEN:{
				fprintf(fp, stack->lexeme);
				lastVar = stack->lexeme;
				pos = ftell(fp) - strlen(stack->lexeme);
				break;
			}
			case FOR_KW:{
				pos = ftell(fp);
				stack = stack->next;
				stack = stack->next;
				if(stack->tokenType == INT_KW){
					stack = stack->next;
					fseek(fp, pos, SEEK_SET);
					char * varName = stack->lexeme;
					fprintf(fp, "int %s = ", varName);
					stack = stack->next;
					stack = stack->next;
					char * varValue = stack->lexeme;
					fprintf(fp, "%s; for(%s = %s", varValue, varName, varValue);
				} else {
					fprintf(fp, "for( %s", stack->lexeme);
				}
				break;
			}
			case READ_FN:{
				fseek(fp, pos, SEEK_SET);
				fprintf(fp, "scanf");
				stack = stack->next;
				stack = stack->next;
				fprintf(fp, "(");
				if(scopeHashTableGetTokenType(gScopeHashTable, lastVar) != -1){
					if(scopeHashTableGetTokenType(gScopeHashTable, lastVar) == INT_KW){
						fprintf(fp, "\"%%d\", &");
					} else if(scopeHashTableGetTokenType(gScopeHashTable, lastVar) == STRING_KW){
						fprintf(fp, "\"%%s\", &");
					}
				} else if(hashTableGetTokenType(gHashTable, lastVar) != -1){
					if(hashTableGetTokenType(gHashTable, lastVar) == INT_KW){
						fprintf(fp, "\"%%d\", &");
					} else if(hashTableGetTokenType(gHashTable, lastVar) == STRING_KW){
						fprintf(fp, "\"%%s\", &");
					}
				}
				fprintf(fp, lastVar);
				fprintf(fp, ")");
				break;
			}
			case STR_LIT:{
				fprintf(fp, "\"");
				fprintf(fp, stack->lexeme);
				fprintf(fp, "\" ");
				break;
			}
			case BOOL_KW:{
				fprintf(fp, "int ");
				break;
			}
			case TRUE_RW:{
				fprintf(fp, "1");
				break;
			}
			case FALSE_RW:{
				fprintf(fp, "0");
				break;
			}
			case STRING_KW:{
				fprintf(fp, "char * ");
				break;
			}
			default:{
				fprintf(fp, stack->lexeme);
				fprintf(fp, " ");
				break;
			}
		}
		stack = stack->next;
	}
	fprintf(fp, "getch(); }");
	fclose(fp);
}
