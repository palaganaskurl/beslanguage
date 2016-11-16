#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\headers\LINKEDLIST.H"
#include "..\headers\LEXER.H"
#include "..\headers\PARSER.H"
#include "..\headers\SYMBOLTABLE.H"
#include "..\headers\CODE_GENERATION.h"

Stack * stack;

HashTable * hashTable;
ScopeHashTable * scopeHashTable;

char * variableName;

TokenTypeBes dataType;

int isSameDataType = 0;

char * arrScopeVariables[100];
int scopeVarCounter = 0;

char * arrAllVariables[100];
int allVarCounter = 0;

int isScope = 0;

int depth = 0;

int lastLine;

char errorList[1000][1000];

int errorCounter = 0;

void initToken(TokenList * tokenList){
	stack = tokenList->first;
	hashTable = hashTableCreate(65536);
	scopeHashTable = scopeHashTableCreate(65536);
}

void errorRecovery(){
	while(1){ // binura ko yung open brace
		if(match(SEMICOLON) || match(CLOSE_PAREN) || match(IF_KW) || match(FOR_KW)
			|| match(DO_KW) || match(INT_KW) || match(BOOL_KW) || match(CHAR_KW) || match(FLOAT_KW) || match(DOUBLE_KW) 
			|| match(STRING_KW) || match(WHILE_KW) || match(ELSE_KW) || match(WRITELN_FN)
			|| match(WRITE_FN) || match(VALID_IDEN)
		)
			break;
		else
			getNextToken();
	}
}

void displayError(int lineNumber, char * errorMsg){
	printf("Line Number %d : %s", lineNumber, errorMsg);
	sprintf(errorList[errorCounter], "Line Number %d : %s", lineNumber, errorMsg);
	errorCounter++;	
	errorRecovery();
}

void getNextToken(){
	stack = stack->next;
}

void checkIfVariableDeclared(char * varName){
	variableName = varName;
	if(isScope){
		if(scopeHashTableGetValue(scopeHashTable, variableName) == NULL && hashTableGetValue(hashTable, variableName) == NULL){	
			sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' is not declared bes!\n", stack->tokenLine, varName);
			errorCounter++;	
		}
	} else {
		if(hashTableGetValue(hashTable, varName) == NULL){
			sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' is not declared bes!\n", stack->tokenLine, varName);
			errorCounter++;
		}
	}
}

void statementsNatural(){
	if(match(IF_KW))	expectConditionals();
	else if(match(FOR_KW)) expectLoop();
	else if(match(DO_KW))	expectLoop();
	else if(match(WHILE_KW))	expectLoop();
	else if(match(WRITE_FN) || match(WRITELN_FN))	expectWrite();
	else if(match(INT_KW) || match(DOUBLE_KW) || match(STRING_KW) || match(CHAR_KW) || match(FLOAT_KW) || match(BOOL_KW))	expectDeclaration();
	else if(match(VALID_IDEN)){
		variableName = getVarName();
		getNextToken();
		expectAssignment();
	} else if(match(WRITE_FN)|| match(WRITELN_FN))	expectWrite();
	else if(match(BREAK_KW)){
		sprintf(errorList[errorCounter], "Line Number %d : Syntax Error. Unexpected break here. No loop!", stack->tokenLine);
		errorCounter++;
		getNextToken();
		if(match(SEMICOLON)) getNextToken();
		else displayError(stack->tokenLine, "Expected semicolon after break!\n");
	}
}

char * getVarName(){
	char * tempLexemeIden = stack->lexeme;
	if(strlen(stack->lexeme) == 1){ // If VALID_IDEN length == 1
		tempLexemeIden[1] = ' ';
		tempLexemeIden[2] = '\0';	
	}
	return tempLexemeIden;
}

void depthFunction(){
	int i;
	for(i = 0; i < scopeVarCounter; i++)
		scopeHashTableRemoveVariable(scopeHashTable, arrScopeVariables[i], depth);
	depth--;
	if(depth == 0){
		memset(arrScopeVariables, 0, 255);
		scopeVarCounter = 0;
		isScope = 0;
	}
}

int match(TokenTypeBes tokenType){
	if(stack->tokenType == tokenType)
		return 1;	
	else
		return 0;
}

void expectArithmetic(){
	getNextToken();
	if(match(NUMBER_LIT) || match(DECIMAL_LIT) || match(VALID_IDEN)){
		if(match(VALID_IDEN))
			checkIfVariableDeclared(getVarName());
		getNextToken();
		if(match(SEMICOLON))
			return;
		else if(match(ARITH_OPER))
			expectArithmetic();
	} else {
		displayError(stack->tokenLine, "Syntax error. Unexpected arithmetic operator bes!\n");
		return;
	}
}

void expectAssignment(){
	char * assignedVariableName;
	getNextToken();
	if(match(NUMBER_LIT) || match(STR_LIT) || match(DECIMAL_LIT) ||match(CHR_LIT) || match(TRUE_RW) || match(FALSE_RW) || match(NULL_RW) || match(READ_FN) || match(VALID_IDEN)){
		/*
		  ----Identifier to Identifier Assignments-----
		  Check if VALID_IDEN (Valid Identifier)
		  If VALID_IDEN doesn't exist in HASHTABLE
		     Return VARIABLE NOT DECLARED
	      Else
	         Check if compatible types
	         If compatible
			    add VALID_IDEN to HASHTABLE 
		*/
		if(match(VALID_IDEN)){
			assignedVariableName = stack->lexeme;
			if(strlen(assignedVariableName) == 1){
				assignedVariableName[1] = ' ';
				assignedVariableName[2] = '\0';
			}
			if(isScope){
				if(scopeHashTableGetValue(scopeHashTable, variableName) == NULL && hashTableGetValue(hashTable, variableName) == NULL){
					sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' is not declared bes!\n", stack->tokenLine, variableName);
					errorCounter++;
				}
				if(scopeHashTableGetValue(scopeHashTable, assignedVariableName) == NULL && hashTableGetValue(hashTable, assignedVariableName) == NULL){
					sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' is not declared bes!\n", stack->tokenLine, assignedVariableName);
					errorCounter++;
				}
				else {
					if(scopeHashTableGetTokenType(scopeHashTable, assignedVariableName) == scopeHashTableGetTokenType(scopeHashTable, variableName)){
						scopeHashTableSet(scopeHashTable, variableName, dataType, scopeHashTableGetValue(scopeHashTable, scopeHashTableGetValue(scopeHashTable, assignedVariableName)), depth);
					} else if(hashTableGetTokenType(hashTable, assignedVariableName) == hashTableGetTokenType(hashTable, variableName)){
						hashTableSet(hashTable, variableName, dataType, scopeHashTableGetValue(scopeHashTable, scopeHashTableGetValue(scopeHashTable, assignedVariableName)));
					} else {
//						if(scopeHashTableGetTokenType(scopeHashTable, assignedVariableName) != -1){
//							sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' has incompatible types bes!\n", stack->tokenLine, assignedVariableName);
//							errorCounter++;
//						}
					}
				}
			} else {
				if(hashTableGetValue(hashTable, variableName) == NULL){
					sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' is not declared bes!\n", stack->tokenLine, variableName);
					errorCounter++;
				}
				if(hashTableGetValue(hashTable, assignedVariableName) == NULL){
					sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' is not declared bes!\n", stack->tokenLine, assignedVariableName);
					errorCounter++;
				}
				else {
					if(hashTableGetTokenType(hashTable, assignedVariableName) == hashTableGetTokenType(hashTable, variableName)){
						hashTableSet(hashTable, variableName, dataType, hashTableGetValue(hashTable, assignedVariableName));
					} else {
						if(hashTableGetTokenType(hashTable, assignedVariableName) != -1){
							sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' has incompatible types bes!\n", stack->tokenLine, assignedVariableName);
							errorCounter++;
						}
					} 
				}	
			}
		} else {
			/*----Integer Assignment Semantics----------
			  ----String Assignment Semantics-----------
			  ----Floating Point Assignment Semantics---
			  ----Boolean Assignment Semantics----------
			  Check if dataType assignedValue is correct
			  If wrong return errorMessage
			  Else add the variable to hashTable
			  ------------------------------------------
			*/
			if(dataType == INT_KW && !match(NUMBER_LIT) && dataType == INT_KW && !match(DECIMAL_LIT) && dataType == INT_KW && !match(VALID_IDEN) && !match(READ_FN)){
				sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' expecting an integer value bes!\n", stack->tokenLine, variableName);
				errorCounter++;
			} else if(dataType == STRING_KW && !match(STR_LIT) && dataType == STRING_KW && !match(VALID_IDEN) && !match(READ_FN)){
				sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' expecting a string value bes!\n", stack->tokenLine, variableName);
				errorCounter++;
			} else if((dataType == DOUBLE_KW && !match(DECIMAL_LIT) || (dataType == FLOAT_KW && !match(DECIMAL_LIT)) && !match(READ_FN))
					 && (dataType == DOUBLE_KW && !match(VALID_IDEN) || (dataType == FLOAT_KW && !match(VALID_IDEN))) && !match(READ_FN)
					 && (dataType == DOUBLE_KW && !match(NUMBER_LIT)) || (dataType == FLOAT_KW && !match(NUMBER_LIT))){
					 	sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' expecting a floating-point value bes!\n", stack->tokenLine, variableName);
					 	errorCounter++;
			} else if(dataType == BOOL_KW && !match(TRUE_RW) && dataType == BOOL_KW && !match(FALSE_RW) && dataType == BOOL_KW && !match(NULL_RW)
					&& dataType == BOOL_KW && !match(VALID_IDEN) && !match(READ_FN)){
						sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' expecting a boolean value bes!\n", stack->tokenLine, variableName);			
						errorCounter++;
			} else if(match(READ_FN)){
				checkIfVariableDeclared(variableName);
				getNextToken();
				if(match(OPEN_PAREN)){
					getNextToken();
					if(match(CLOSE_PAREN)){
						getNextToken();
						if(match(SEMICOLON)){
							getNextToken();
							statementsNatural();
							return;
						} else {
							displayError(stack->tokenLine, "Syntax error. Expecting semicolon bes!\n");
							return;		
						}
					} else {
						displayError(stack->tokenLine, "Syntax error. Expecting close parenthesis bes!\n");
						return;	
					}
				} else {
					displayError(stack->tokenLine, "Syntax error. Expecting open parenthesis bes!\n");
					return;
				}
			} else {
				if(isScope){
					if(scopeHashTableGetValue(scopeHashTable, variableName) == NULL && hashTableGetValue(hashTable, variableName) == NULL){
						sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' is not declared bes!\n", stack->tokenLine, variableName);
						errorCounter++;
					} else
						scopeHashTableSet(scopeHashTable, variableName, dataType, stack->lexeme, depth);
				} else {
					if(hashTableGetValue(hashTable, variableName) == NULL){
						sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' is not declared bes!\n", stack->tokenLine, variableName);
						errorCounter++;
					} else
						hashTableSet(hashTable, variableName, dataType, stack->lexeme);
				}
			}
		}
		if(match(NUMBER_LIT) || match(DECIMAL_LIT) || match(VALID_IDEN)){
			getNextToken();
			if(match(ARITH_OPER))
				expectArithmetic();
		} else getNextToken();
	} else {
		displayError(stack->tokenLine, "Syntax error. Expecting a value bes!\n");
		return;
	}
		
	if(match(COMMA)){ // int x = 1, y;
		isSameDataType = 1;
		expectDeclaration();	
		return;
	}
	
	if(match(SEMICOLON)){
		isSameDataType = 0;
		getNextToken();	
		return;
	} else {
		displayError(stack->tokenLine, "Syntax error. Expecting a semicolon bes!\n");
		return;
	}
}

void expectDeclaration(){
	TokenTypeBes tempDataType;
	if(!isSameDataType)
		tempDataType = stack->tokenType;
	getNextToken();
	if(!match(VALID_IDEN)){ // e.g. int = 1;
		displayError(stack->tokenLine, "Syntax error. Expecting identifier bes!\n");
		return;
	} else if(match(COMMA)){ // e.g. int , 1;
		displayError(stack->tokenLine, "Syntax error. Unexpected symbol after identifier bes!\n");
		return;
	} else { // int -> int x 
		/*------I don't know what bug is this pa.-----------
		  ------Maybe hashtable bug.------------------------
		  ------Kapag isang letter lang, dagdagan ng space--
		  ------Para mabasa---------------------------------
		*/
		variableName = getVarName(); // note : nilipat ko ito mula sa loob nung if 
		int i, flag = 0;
		for(i = 0; i < allVarCounter; i++){
			if(strcmp(variableName, arrAllVariables[i]) == 0){
				flag = 1;
				break;
			}
		}
		if(!flag){
			arrAllVariables[allVarCounter] = variableName;
			allVarCounter++;
		}
		if(isScope){
			if(scopeHashTableGetValue(scopeHashTable, variableName) == NULL){
				scopeHashTableSet(scopeHashTable, variableName, tempDataType, "", depth);
				arrScopeVariables[scopeVarCounter] = variableName;
				scopeVarCounter++;
			} else {
				sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' is not declared bes!\n", stack->tokenLine, variableName);
				errorCounter++;
			}
		} else {
			if(hashTableGetValue(hashTable, variableName) == NULL){
				hashTableSet(hashTable, variableName, tempDataType, "");
			} else {
				sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' is not declared bes!\n", stack->tokenLine, variableName);
				errorCounter++;
			}
		}
		getNextToken();
	}
		
	if(match(VALID_IDEN)){ // int x x;
		displayError(stack->tokenLine, "Syntax error. Unexpected identifier bes!\n");
		expectDeclaration();
		return;
	}
	
	if(match(ASS_OPER) || match(ASS_COMP_OPER)){ // int x = 
		if(!isSameDataType)
			dataType = tempDataType;
		expectAssignment();
		return;
	} else if(match(SEMICOLON)){
		isSameDataType = 0;
		getNextToken();
		return;
	} else if(match(NUMBER_LIT) || match(STR_LIT) || match(CHR_LIT) || match(TRUE_RW) || match(FALSE_RW || match(NULL_RW))){
		displayError(stack->tokenLine, "Syntax error. Expecting an assignment operator bes!\n");
		getNextToken();
		if(match(SEMICOLON)){
			getNextToken();
			return;
		}
	}
	
	if(match(COMMA)){
		expectDeclaration();
		return;
	}
	
	if(match(SEMICOLON)){
		isSameDataType = 0;
		getNextToken();	
	} else {
		displayError(stack->tokenLine, "Syntax Error. Expecting a semicolon bes!\n");
		return;
	}
}

void expectUnaryOperation(){
	getNextToken();
	if(!match(VALID_IDEN)){
		displayError(stack->tokenLine, "Syntax error. Expecting identifier before unary operator bes.\n");
		return;
	} else {
		checkIfVariableDeclared(getVarName());
		getNextToken();
	}
		
	if(match(UNA_OPER) || match(ASS_COMP_OPER)){
		if(match(UNA_OPER)){
			getNextToken();
			return;
		}	
		if(match(ASS_COMP_OPER)){
			getNextToken();
			if(match(NUMBER_LIT) || match(DECIMAL_LIT)){
				getNextToken();
				return;
			} else {
				displayError(stack->tokenLine, "Syntax error. Expecting expression bes!\n");
				return;	
			}
		}
	} else {
		displayError(stack->tokenLine, "Syntax error. Expecting expression bes!\n");
		return;	
	}		
}

void expectCondition(){
	int mustBeValidIntEquiv = 0;
	TokenTypeBes firstToken = stack->tokenType, secondToken;
	if((match(NUMBER_LIT) || match(DECIMAL_LIT) || match(VALID_IDEN) || match(STR_LIT) || match(CHR_LIT) || match(TRUE_RW) || match(FALSE_RW) || match(NULL_RW))){
		if(match(VALID_IDEN)){
			checkIfVariableDeclared(getVarName());
			if(isScope){
				if(scopeHashTableGetValue(scopeHashTable, getVarName()) != NULL)
					firstToken = scopeHashTableGetTokenType(scopeHashTable, getVarName());
				else if(hashTableGetValue(hashTable, getVarName()) != NULL)
					firstToken = hashTableGetTokenType(hashTable, getVarName());
			} else {
				if(hashTableGetValue(hashTable, getVarName()) != NULL)
					firstToken = hashTableGetTokenType(hashTable, getVarName());
			}	
		}
		getNextToken();
		if(match(CLOSE_PAREN)){
			if(hashTableGetTokenType(hashTable, variableName) != BOOL_KW){
				sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' does not contain boolean value. Cannot be an expression!\n", stack->tokenLine, variableName);
				errorCounter++;
			}
			return;
		}
			
	} else {
		displayError(stack->tokenLine, "Syntax error. Expecting condition bes!\n");
		return;
	}
		
	if(!match(REL_OPER)){
		displayError(stack->tokenLine, "Syntax error. Expecting condition bes!\n");
		return;
	} else{
		if(strcmp(stack->lexeme, "<=") == 0 || strcmp(stack->lexeme, ">=") == 0 || strcmp(stack->lexeme, "<") == 0 || strcmp(stack->lexeme, ">") == 0)
			mustBeValidIntEquiv = 1;
		getNextToken();
	}
	secondToken = stack->tokenType;
	if(match(NUMBER_LIT) || match(DECIMAL_LIT) || match(VALID_IDEN) || match(STR_LIT) || match(CHR_LIT) || match(TRUE_RW) || match(FALSE_RW) || match(NULL_RW)){
		if(match(VALID_IDEN)){
			checkIfVariableDeclared(getVarName());
			if(isScope){
				if(scopeHashTableGetValue(scopeHashTable, getVarName()) != NULL)
					secondToken = scopeHashTableGetTokenType(scopeHashTable, getVarName());
				else if(hashTableGetValue(hashTable, getVarName()) != NULL)
					secondToken = hashTableGetTokenType(hashTable, getVarName());
			} else {
				if(hashTableGetValue(hashTable, getVarName()) != NULL)
					secondToken = hashTableGetTokenType(hashTable, getVarName());
			}
		}
		if(mustBeValidIntEquiv){
			if(firstToken == STR_LIT || firstToken == CHR_LIT || firstToken == BOOL_KW || firstToken == TRUE_RW || firstToken == FALSE_RW || firstToken == STRING_KW || firstToken == CHAR_KW
				|| secondToken == STR_LIT || secondToken == CHR_LIT || secondToken == BOOL_KW || secondToken == TRUE_RW || secondToken == FALSE_RW || secondToken == STRING_KW || secondToken == CHAR_KW){
					sprintf(errorList[errorCounter], "Line Number %d : Warning. Illegal comparison bes!\n", stack->tokenLine);
					errorCounter++;
				}
		}
		getNextToken();
	} else {
		displayError(stack->tokenLine, "Syntax error. Expecting condition bes!\n");
		return;
	}
	
	if(match(LOG_OPER)){
		getNextToken();
		expectCondition();
	} else 
		return;
}

void elseIf(){
	if(match(IF_KW)){
		getNextToken();
		if(!match(OPEN_PAREN)){
			displayError(stack->tokenLine, "Syntax error. Expecting open parenthesis bes!\n");
			return;
		} else
			getNextToken();
		expectCondition();
		if(!match(CLOSE_PAREN)){
			displayError(stack->tokenLine, "Syntax error. Expecting close parenthesis bes!\n");
			return;
		} else 
			getNextToken();
			
		if(!match(OPEN_BRACE)){
			displayError(stack->tokenLine, "Syntax error. Expecting open brace bes!\n");
			return;
		} else
			getNextToken();
		
		while(!match(CLOSE_BRACE))
			statementsNatural();
		
		if(!match(CLOSE_BRACE)){
			displayError(stack->tokenLine, "Syntax error. Expecting close brace bes!\n");
			return;
		} else {
			getNextToken();
			if(match(ELSE_KW)){
				getNextToken();
				if(match(IF_KW)){
					elseIf();
					return;
				}
				
				if(!match(OPEN_BRACE)){
					displayError(stack->tokenLine, "Syntax error. Expecting open brace after else bes!\n");
					return;
				} else if(match(OPEN_BRACE)){
					getNextToken();
					while(!match(CLOSE_BRACE))
						statementsNatural();
					if(!match(CLOSE_BRACE)){
						displayError(stack->tokenLine, "Syntax error. Expecting close brace bes!\n");
						return;
					}
				}
			}
		}
	}
}

/*
	if {
	
	} else if(){
	
	} else {
	
	} else if(){
	
	}
*/

void expectConditionals(){
	TokenTypeBes lastKeyword;
	if(match(IF_KW)){
		lastKeyword = IF_KW;
		getNextToken();
		if(!match(OPEN_PAREN)){
			displayError(stack->tokenLine, "Syntax error. Expecting open parenthesis bes!\n");
			return;
		} else
			getNextToken();
		expectCondition();
		if(!match(CLOSE_PAREN)){
			displayError(stack->tokenLine, "Syntax error. Expecting close parenthesis bes!\n");
			return;
		} else 
			getNextToken();
			
		if(!match(OPEN_BRACE)){
			displayError(stack->tokenLine, "Syntax error. Expecting open brace bes!\n");
			return;
		} else
			getNextToken();
		
		isScope = 1;
		depth++;
		
		while(!match(CLOSE_BRACE))
			statementsNatural();
		
		int i;
		for(i = 0; i < scopeVarCounter; i++){
			scopeHashTableRemoveVariable(scopeHashTable, arrScopeVariables[i], depth);
		}
		isScope = 0;
		scopeVarCounter = 0;
		memset(arrScopeVariables, 0, 255);

		if(!match(CLOSE_BRACE)){
			displayError(stack->tokenLine, "Syntax error. Expecting close brace bes!\n");
			return;
		} else {
			depthFunction();
			getNextToken();
			if(match(ELSE_KW)){
				lastKeyword = ELSE_KW;
				getNextToken();
				if(match(IF_KW)){ // else if
					lastKeyword = IF_KW;
					elseIf();
					return;
				} else if(match(OPEN_BRACE)){ // else
					depth++;
					getNextToken();
					
					if(match(IF_KW))	expectConditionals();
					else if(match(FOR_KW)) expectLoop();
					else if(match(DO_KW))	expectLoop();
					else if(match(WHILE_KW))	expectLoop();
					else if(match(WRITE_FN) || match(WRITELN_FN))	expectWrite();
					else if(match(INT_KW) || match(DOUBLE_KW) || match(STRING_KW) || match(CHAR_KW) || match(FLOAT_KW) || match(BOOL_KW))	expectDeclaration();
					else if(match(VALID_IDEN)){
						getNextToken();
						expectAssignment();
					} else if(match(WRITE_FN)|| match(WRITELN_FN))	expectWrite();
					
					if(!match(CLOSE_BRACE)){
						displayError(stack->tokenLine, "Syntax error. Expecting close brace bes!\n");
						return;
					} else {
						depthFunction();
						getNextToken();
						if(match(ELSE_KW)){
							lastKeyword = ELSE_KW;
							displayError(stack->tokenLine, "Syntax error. Else if after else!\n");
							return;	
						}
					}
					return;
				} else if(!match(OPEN_BRACE)){
					displayError(stack->tokenLine, "Syntax error. Else not expecting any form of expressions bes!\n");
			 		return;
			 	}
			} else if(match(IF_KW)){
				lastKeyword = IF_KW;
				expectConditionals();
				return;
			}
		}
	}
}

void expectConcat(){
	if(match(CONCAT_OPER)){
		getNextToken();	
		if(match(STR_LIT) || match(VALID_IDEN) || match(NUMBER_LIT) || match(CHR_LIT) || match(DECIMAL_LIT)){
			if(match(VALID_IDEN)){
				variableName = getVarName();
				if(isScope){
					if(scopeHashTableGetValue(scopeHashTable, variableName) == NULL && hashTableGetValue(hashTable, variableName) == NULL){
						sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' is not declared bes!\n", stack->tokenLine, variableName);
						errorCounter++;
					}
				} else {
					if(hashTableGetValue(hashTable, variableName) == NULL){
						sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' is not declared bes!\n", stack->tokenLine, variableName);
						errorCounter++;
					}
				}
				getNextToken();
			} else getNextToken();
			if(match(CONCAT_OPER) || match(CLOSE_PAREN)){
				expectConcat();
				return;	
			} else {
				displayError(stack->tokenLine, "Syntax error. Missing concat operator bes!\n");
				while(1){
					if(match(SEMICOLON))
						break;
					getNextToken();
				}
				return;
			}
		} else {
			displayError(stack->tokenLine, "Syntax error. Concat error bes!\n");
			return;
		}
	}
}

void expectWrite(){
	if(match(WRITE_FN) || match(WRITELN_FN)) getNextToken();
	else return;
	if(match(OPEN_PAREN)) getNextToken();
	else{
		displayError(stack->tokenLine, "Syntax error. Expecting open parenthesis bes!\n");
		return;
	}
	while(1){
		if(match(CLOSE_PAREN))
			break;
		if(match(STR_LIT) || match(VALID_IDEN) || match(NUMBER_LIT) || match(CHR_LIT) || match(DECIMAL_LIT)){
			if(match(VALID_IDEN)){
				variableName = getVarName();
				if(isScope){
					if(scopeHashTableGetValue(scopeHashTable, variableName) == NULL && hashTableGetValue(hashTable, variableName) == NULL){
						sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' is not declared bes!\n", stack->tokenLine, variableName);
						errorCounter++;
					} else{
						if(scopeHashTableGetValue(scopeHashTable, variableName) != NULL)
							scopeHashTableSetIsUsed(scopeHashTable, variableName);
						else if(hashTableGetValue(hashTable, variableName) != NULL)
							hashTableSetIsUsed(hashTable, variableName);
					}
						
				} else {
					if(hashTableGetValue(hashTable, variableName) == NULL){
						sprintf(errorList[errorCounter], "Line Number %d : Variable \'%s\' is not declared bes!\n", stack->tokenLine, variableName);
						errorCounter++;
					} else
						hashTableSetIsUsed(hashTable, variableName);
				}
				getNextToken();
			} else getNextToken();
				
			if(match(CONCAT_OPER)){
				expectConcat();
			} else if(match(CLOSE_PAREN))	break;
			else {
				displayError(stack->tokenLine, "Syntax error. Unexpected token bes!\n");
				return;
			}
		} else {
			getNextToken();
			return;
		}
	}
	if(match(CLOSE_PAREN))	getNextToken();
	else{
		displayError(stack->tokenLine, "Syntax error. Expecting close parenthesis!\n");
		return;
	}
	if(match(SEMICOLON)){
		getNextToken();
		statementsNatural();
		return;	
	} else {
		displayError(stack->tokenLine, "Syntax error. Expecting semicolon bes.\n");
		return;
	}
}

void expectLoop(){
	if(match(FOR_KW)){ // for
		isScope = 1;
		getNextToken();
		if(!match(OPEN_PAREN)){
			displayError(stack->tokenLine, "Syntax error. Expecting open parenthesis bes!\n");
			return;
		} else getNextToken();
		
		depth++;
			
		if(match(INT_KW) || match(DOUBLE_KW) || match(STRING_KW) || match(CHAR_KW) || match(FLOAT_KW) || match(BOOL_KW))
			expectDeclaration();
		else if(match(VALID_IDEN)){
			variableName = getVarName();
			getNextToken();
			expectAssignment();
		}
		
		expectCondition();
		
		if(!match(SEMICOLON)){
			displayError(stack->tokenLine, "Syntax error. Expecting semicolon bes!\n");
			return;
		} else	expectUnaryOperation();
		
		if(!match(CLOSE_PAREN)){
			displayError(stack->tokenLine, "Syntax error. Expecting close parenthesis bes!\n");
			return;
		} else {
			lastLine = stack->tokenLine;
			getNextToken();	
		}
		
		if(!match(OPEN_BRACE)){
			displayError(lastLine, "Syntax error. Expecting open brace bes!\n");
			return;
		} else
			getNextToken();
	
		while(!match(CLOSE_BRACE)){
			lastLine = stack->tokenLine;
			if(match(FOR_KW)) expectLoop();
			else if(match(IF_KW))	expectConditionals();
			else if(match(DO_KW)){
				expectLoop();
				getNextToken();	
			} else if(match(WHILE_KW))	expectLoop();
			else if(match(WRITE_FN) || match(WRITELN_FN))	expectWrite();
			else if(match(VALID_IDEN)){
				variableName = getVarName();
				getNextToken();
				expectAssignment();
			} else if(match(INT_KW) || match(DOUBLE_KW) || match(STRING_KW) || match(CHAR_KW) || match(FLOAT_KW) || match(BOOL_KW))	expectDeclaration();
		}
		
		if(!match(CLOSE_BRACE)){
			displayError(lastLine, "Syntax error. Expecting close brace bes!\n");
			return;
		} else {
			depthFunction();
			getNextToken();
		}
	} else if(match(DO_KW)){ // do while
		isScope = 1;
		getNextToken();
		if(!match(OPEN_BRACE)){
			displayError(stack->tokenLine, "Syntax error. Expecting open brace bes!\n");
			return;
		} else getNextToken();
		
		depth++;

		while(!match(CLOSE_BRACE)){
			if(match(FOR_KW)) expectLoop();
			else if(match(DO_KW))	expectLoop();
			else if(match(WHILE_KW))	expectLoop();
			else if(match(IF_KW))	expectConditionals();
			else if(match(WRITE_FN) || match(WRITELN_FN))	expectWrite();
			else if(match(VALID_IDEN)){
				variableName = getVarName();
				getNextToken();
				expectAssignment();
			}
			else if(match(INT_KW) || match(DOUBLE_KW) || match(STRING_KW) || match(CHAR_KW) || match(FLOAT_KW) || match(BOOL_KW))	expectDeclaration();
		}
		
		if(!match(CLOSE_BRACE)){
			displayError(stack->tokenLine, "Syntax error. Expecting close brace bes!\n");
			return;
		} else 	{
			getNextToken();
		}
		if(!match(WHILE_KW)){
			displayError(stack->tokenLine, "Syntax error. Do expecting a while bes!\n");
			return;
		} else	getNextToken();
		if(!match(OPEN_PAREN)){
			displayError(stack->tokenLine, "Syntax error. Expecting open parenthesis bes!\n");
			return;
		} else{
			getNextToken();	
			expectCondition();
		}
		if(!match(CLOSE_PAREN)){
			displayError(stack->tokenLine, "Syntax error. Expecting close parenthesis bes!\n");
			return;
		} else{
			getNextToken();
			if(!match(SEMICOLON)){
				displayError(stack->tokenLine, "Syntax error. Expecting delimiter bes!\n");
				return;	
			} 
			depthFunction();
			getNextToken();	
		}	
	} else if(match(WHILE_KW)){ // while
		isScope = 1;
		getNextToken();
		if(!match(OPEN_PAREN)){
			displayError(stack->tokenLine, "Syntax error. Expecting open parenthesis bes!\n");
			return;
		} else	getNextToken();
			
		expectCondition();
		
		if(!match(CLOSE_PAREN)){
			displayError(stack->tokenLine, "Syntax error. Expecting close parenthesis bes!\n");
			return;
		} else	getNextToken();
		
		if(!match(OPEN_BRACE)){
			displayError(stack->tokenLine, "Syntax error. Expecting open brace bes!\n");
			return;
		} else	getNextToken();
		
		depth++;
		
		while(!match(CLOSE_BRACE)){
			if(match(FOR_KW)) expectLoop();
			else if(match(IF_KW))	expectConditionals();
			else if(match(DO_KW)){
				expectLoop();
				getNextToken();
			} else if(match(WHILE_KW))	expectLoop();
			else if(match(WRITE_FN) || match(WRITELN_FN))	expectWrite();
			else if(match(VALID_IDEN)){
				variableName = getVarName();
				getNextToken();
				expectAssignment();
			} else if(match(INT_KW) || match(DOUBLE_KW) || match(STRING_KW) || match(CHAR_KW) || match(FLOAT_KW) || match(BOOL_KW))	expectDeclaration();
		}
		
		if(!match(CLOSE_BRACE)){
			displayError(stack->tokenLine, "Syntax error. Expecting close brace bes!\n");
			return;
		} else {
			depthFunction();
			getNextToken();
		}
	}
}

void readToken(){
	while(stack->next != NULL){
		if(match(INT_KW) || match(DOUBLE_KW) || match(STRING_KW) || match(CHAR_KW) || match(FLOAT_KW) || match(BOOL_KW))	expectDeclaration();
		else if(match(FOR_KW))	expectLoop(); 
		else if(match(DO_KW))	expectLoop();
		else if(match(WHILE_KW)) expectLoop();
		else if(match(IF_KW))	expectConditionals();
		else if(match(WRITE_FN)|| match(WRITELN_FN))	expectWrite();
		else if(match(VALID_IDEN)){
			isScope = 0;
			variableName = getVarName();
			getNextToken();
			expectAssignment();
		}	
		else 
		getNextToken();
	}
	initHashTable(hashTable, scopeHashTable);
	errorCounter = 0;
//	int i;
//	for(i = 0; i < allVarCounter; i++){
//		if(hashTableGetIsUsed(hashTable, arrAllVariables[i]) == 0){
//			printf("Variable \'%s\' is not used.\n", arrAllVariables[i]);
//		}
//	}
//	hashTableSet(hashTable, "lolohaha", INT_KW, "12");
//	scopeHashTableRemoveVariable(scopeHashTable, "y ");
//	printf("C : %s\n", hashTableGetValue(hashTable, "y "));
//	printf("C : %d\n", hashTableGetTokenType(hashTable, "y "));	
//	printf("D : %d\n", scopeHashTableGetValue(scopeHashTable, "y "));
}




