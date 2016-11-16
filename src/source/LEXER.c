#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "..\headers\LINKEDLIST.H"
#include "..\headers\LEXER.H"

char arrOperators[] = {'+','-','*','/','%','<','>','!','|','&','=','.', '^'};
char arrDelimiters[] = {',','{','}','[',']',';','(',')'};

int lineNumber;

TokenList * tokenList;

TokenList * createTokenList(){
	tokenList = list_create();
	return tokenList;
}

void checkNewLine(char fch){
	if(fch == '\n')
		lineNumber++;
}

void initLexer(FILE *fp, FILE *fo){
	lineNumber = 1;
	char fch; // each character in file
	while((fch = fgetc(fp)) != EOF){
		if(fch == '#')
			isComment(fch, fp, fo);
		else if(isalpha(fch) || fch == '@')
			isKeyword(fch, fp, fo);
		else if(isDelim(fch))
			isDelimiter(fch, fp, fo);
		else if(isOper(fch))
			isOperator(fch, fp, fo);
		else if(isdigit(fch))
			isNumberLiteral(fch, fp, fo);
		else if(fch == '"')
			isStringLiteral(fch, fp, fo);
		else if(fch == '\'')
			isCharacterLiteral(fch, fp, fo);
		else{
			if(!isspace(fch))
				printf("%c\t\tNOISE_WORD\n", fch);
		}
		if(fch == '\n')
			checkNewLine(fch);
	}
	fclose(fo);
	FILE * foo = fopen("SYMBOL TABLE.txt", "r");
	getLexemes(foo);
}

void isComment(char fch, FILE *fp, FILE *fo){
	char lch; // last character
	if(fch == '#'){
		printf("%c", fch);
		fprintf(fo, "%c", fch);
		fch = fgetc(fp);
		if(fch == '#'){
			printf("%c", fch);
			fprintf(fo, "%c", fch);
			while(1){
				if(fch == '#' && lch == '#'){
					fch = fgetc(fp);
					if(fch == '\n'){
						fileOutput(fo, "BLOCK_COMMENT");
						return;
					}		
				}
				lch = fch;
				fch = fgetc(fp);
				printf("%c", fch);
				fprintf(fo, "%c", fch);
			}
		}
		while(1){
			printf("%c", fch);
			fprintf(fo, "%c", fch);
			fch = fgetc(fp);
			if(fch == '\n'){
				fch = fgetc(fp);
				fileOutput(fo, "SINGLE_COMMENT");
				return;
			}
		}
		return;
	}
}

void isOperator(char fch, FILE *fp, FILE *fo){
	char lch;
	if(isOper(fch)){
		printf("%c", fch);
		fprintf(fo, "%c", fch);
		lch = fch;
		fch = fgetc(fp);
		if(!isOper(fch)){
			if(lch == '='){
				fileOutput(fo, "ASS_OPER");
				listAppend(tokenList, ASS_OPER, lineNumber);
			}
			else if(lch == '+' || lch == '-' || lch == '*' || lch == '/' || lch == '%' || lch == '^'){
				fileOutput(fo, "ARITH_OPER");
				listAppend(tokenList, ARITH_OPER, lineNumber);
			} else if(lch == '<' || lch == '>'){
				fileOutput(fo, "REL_OPER");
				listAppend(tokenList, REL_OPER, lineNumber);
			} else if(lch == '.'){
            	fileOutput(fo, "CONCAT_OPER");
            	listAppend(tokenList, CONCAT_OPER, lineNumber);
			} else if(lch == '!'){
				fileOutput(fo, "LOG_OPER");
				listAppend(tokenList, LOG_OPER, lineNumber);
			}
			if(isspace(fch))
				outputSpace(fo);
			if(isdigit(fch))
				isNumberLiteral(fch,fp,fo);
			return;
		}
		if((lch == '&' && fch == '&') || (lch == '|' && fch == '|') || (fch == '!')){
			fileOutputX(fo, "LOG_OPER", fch);
			listAppend(tokenList, LOG_OPER, lineNumber);
		} else if((lch == '<' && fch == '=') || (lch == '>' && fch == '=') || (lch == '!' && fch == '=') || (lch == '=' && fch == '=')){
			fileOutputX(fo, "REL_OPER", fch);
			listAppend(tokenList, REL_OPER, lineNumber);
		} else if((lch == '+' && fch == '+') || (lch == '-' && fch == '-')){
			fileOutputX(fo, "UNA_OPER", fch);
			listAppend(tokenList, UNA_OPER, lineNumber);
		} else if((lch == '+' && fch == '=') || (lch == '-' && fch == '=') || (lch == '*' && fch == '=') || (lch == '/' && fch == '=') || (lch == '%' && fch == '=')){
			fileOutputX(fo, "ASS_COMP_OPER", fch);
			listAppend(tokenList, ASS_COMP_OPER, lineNumber);
		} else if(isdigit(fch))
			isNumberLiteral(fch,fp, fo);
		return;
	} else
		return;
}

void isStringLiteral(char fch, FILE *fp, FILE *fo){
	if(fch == '"'){
		fch = fgetc(fp);
		if(isspace(fch)){
			printf("%c", fch);
			fprintf(fo, "%c", fch);
		}
		while(1){
			if(fch == '"')
				break;
            printf("%c", fch);
            fprintf(fo, "%c", fch);
            fch = fgetc(fp);
            if(fch == '\\'){
                fileOutput(fo, "STR_LIT");
                listAppend(tokenList, STR_LIT, lineNumber);
                isEscapeSequence(fch, fp, fo);
                while(1){
                    if(fch == '\\'){
                        fch = isEscapeSequence(fch, fp, fo);
                        break;
                    }
                    if(fch == '"')
                        return;
                }
            }
		}
		fileOutput(fo, "STR_LIT");
		listAppend(tokenList, STR_LIT, lineNumber);
		return;
	} else
		return;
}

void isCharacterLiteral(char fch, FILE *fp, FILE *fo){
	if(fch == '\''){
		fch = fgetc(fp);
		while(1){
			if(fch == '\'')
				break;
			fileOutputX(fo, "CHR_LIT", fch);
			listAppend(tokenList, CHR_LIT, lineNumber);
			fch = fgetc(fp);
		}
		return;
	} else
		return;
}

char isEscapeSequence(char fch, FILE *fp, FILE *fo){
	if(fch == '\\'){
        printf("%c", fch);
        fprintf(fo, "%c", fch);
        fch = fgetc(fp);
        if(fch == 'n' || fch == 't' || fch == '\'' || fch == '\\'){
            fileOutputX(fo, "ESC_SEQ", fch);
            listAppend(tokenList, ESC_SEQ, lineNumber);
        } else{
        	fileOutputX(fo, "ILL_ESC_SEQ", fch);
		}
		return fgetc(fp);
	} else
		return 0;
}

void isNumberLiteral(char fch, FILE *fp, FILE *fo){
	int decCount = 0;
	if(isdigit(fch)){
		while(1){
            printf("%c", fch);
            fprintf(fo, "%c", fch);
			fch = fgetc(fp);
			if(isspace(fch) || isDelim(fch) || (isOper(fch) && fch != '.')){
                if(decCount > 1)
                    fileOutput(fo, "ILL_NUM_LIT");
				else if(decCount == 1){
                	fileOutput(fo, "DECIMAL_LIT");
                	listAppend(tokenList, DECIMAL_LIT, lineNumber);
				} else {
                	fileOutput(fo, "NUMBER_LIT");
                	listAppend(tokenList, NUMBER_LIT, lineNumber);
				}
				if(isOper(fch) && fch != '.')
					isOperator(fch,fp, fo);
				if(isDelim(fch))
					isDelimiter(fch, fp, fo);
				break;
			}
            if(fch == '.'){
                decCount++;
            }
		}
		checkNewLine(fch);
		return;
	} else
		return;
}

void isDelimiter(char fch, FILE *fp, FILE *fo){
	if(isDelim(fch)){
		if(fch == '('){
			fileOutputX(fo, "OPEN_PAREN", fch);
			listAppend(tokenList, OPEN_PAREN, lineNumber);
		} else if(fch == ')'){
			fileOutputX(fo, "CLOSE_PAREN", fch);
			listAppend(tokenList, CLOSE_PAREN, lineNumber);	
		} else if(fch == '{'){
			fileOutputX(fo, "OPEN_BRACE", fch);
			listAppend(tokenList, OPEN_BRACE, lineNumber);
		} else if(fch == '}'){
			fileOutputX(fo, "CLOSE_BRACE", fch);
			listAppend(tokenList, CLOSE_BRACE, lineNumber);
		} else if(fch == '['){
			fileOutputX(fo, "OPEN_BRACKET", fch);
			listAppend(tokenList, OPEN_BRACKET, lineNumber);
		} else if(fch == ']'){
			fileOutputX(fo, "CLOSE_BRACKET", fch);
			listAppend(tokenList, CLOSE_BRACKET, lineNumber);
		} else if(fch == ';'){
			fileOutputX(fo, "SEMICOLON", fch);
			listAppend(tokenList, SEMICOLON, lineNumber);
		} else if(fch == ','){
			fileOutputX(fo, "COMMA", fch);
			listAppend(tokenList, COMMA, lineNumber);
		}
		return;
	} else
		return;
}

char getNextCharacter(FILE *fp, FILE *fo, char fch){
	printf("%c", fch);
	fprintf(fo, "%c", fch);
	return fgetc(fp);
}

void fileOutput(FILE *fo, const char *TokenTypeBesBesBesBesBesBesBesBes){
	printf("\t\t%s\t\t Line Number : %d\n", TokenTypeBesBesBesBesBesBesBesBes, lineNumber);
	fprintf(fo, "|\t\t%s\t\t| Line Number : %d\n", TokenTypeBesBesBesBesBesBesBesBes, lineNumber);
}

void fileOutputX(FILE *fo, const char *TokenTypeBesBesBesBesBesBesBesBes, char fch){
	printf("%c\t\t%s\t\t Line Number : %d\n", fch, TokenTypeBesBesBesBesBesBesBesBes, lineNumber);
	fprintf(fo, "%c|\t\t%s|\t\t| Line Number : %d\n", fch, TokenTypeBesBesBesBesBesBesBesBes, lineNumber);
}

void outputSpace(FILE *fo){
	printf("\t\tWHTSPC\n");
}

void isKeyword(char fch, FILE *fp, FILE *fo){
	int validIden = 0, passedNotKeyword = 0, encounteredSpecialSymbols = 0;
	if(isalpha(fch) || fch == '@'){
		while(1){
			if(isalpha(fch) || fch == '_' || isdigit(fch))
				validIden = 1;
			else
				encounteredSpecialSymbols = 1;
			if(fch == 'b'){ // BOOLEAN KEYWORD
				fch = getNextCharacter(fp, fo, fch);
				if(fch == 'o'){ 
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'o'){
					fch = getNextCharacter(fp, fo, fch);	
				if(fch == 'l'){
					fch = getNextCharacter(fp, fo, fch);	
				if(fch == 'e'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'a'){
					fch = getNextCharacter(fp, fo, fch);	
				if(fch == 'n'){
					fch = getNextCharacter(fp, fo, fch);
				if(isspace(fch)){
					fileOutput(fo, "BOOL_KW");
					listAppend(tokenList, BOOL_KW, lineNumber);
					outputSpace(fo);
					return;
				}}}}}}} else if(fch == 'r'){
					fch = getNextCharacter(fp, fo, fch);
					if(fch == 'e'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'a'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'k'){
						fch = getNextCharacter(fp, fo, fch);
						if(isspace(fch)){
						fileOutput(fo, "BREAK_KW");
						listAppend(tokenList, BREAK_KW, lineNumber);
						outputSpace(fo);
						return;
					} else if(isDelim(fch)){
						fileOutput(fo, "BREAK_KW");
						listAppend(tokenList, BREAK_KW, lineNumber);
						isDelimiter(fch, fp, fo);
						return;
				}}}}}
			} else if(fch == 'c'){ // CASE KEYWORD
				fch = getNextCharacter(fp, fo, fch);
				if(fch == 'a'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 's'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'e'){
					fch = getNextCharacter(fp, fo, fch);
				if(isspace(fch)){
					fileOutput(fo, "CASE_KW");
					listAppend(tokenList, CASE_KW, lineNumber);
					outputSpace(fo);
					return;
				}}}
				} else if(fch == 'h'){ // CHAR KEYWORD
					fch = getNextCharacter(fp, fo, fch);
					if(fch == 'a'){ 
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'r'){
						fch = getNextCharacter(fp, fo, fch);
					if(isspace(fch)){
						fileOutput(fo, "CHAR_KW");
						listAppend(tokenList, CHAR_KW, lineNumber);
						outputSpace(fo);
						return;
					}}}
				} else if(fch == 'o'){ // CONTINUE KEYWORD
					fch = getNextCharacter(fp, fo, fch);
					if(fch == 'n'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 't'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'i'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'n'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'u'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'e'){
						fch = getNextCharacter(fp, fo, fch);
					if(isspace(fch)){
						fileOutput(fo, "CONTINUE_KW");
						listAppend(tokenList, CONTINUE_KW, lineNumber);
						outputSpace(fo);
						return;
					} else if(isDelim(fch)){
						fileOutput(fo, "CONTINUE_KW");
						listAppend(tokenList, CONTINUE_KW, lineNumber);
						isDelimiter(fch, fp, fo);
						return;
					}}}}}}}
				}
			} else if(fch == 'd'){ // DEFAULT KEYWORD
				fch = getNextCharacter(fp, fo, fch);
				if(fch == 'e'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'f'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'a'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'u'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'l'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 't'){
					fch = getNextCharacter(fp, fo, fch);
				if(isspace(fch)){
					fileOutput(fo, "DEFAULT_KW");
					listAppend(tokenList, DEFAULT_KW, lineNumber);
					outputSpace(fo);
					return;
				} else if(isDelim(fch)){
					fileOutput(fo, "DEFAULT_KW");
					listAppend(tokenList, DEFAULT_KW, lineNumber);
					isDelimiter(fch, fp, fo);
					return;
				}}}}}}
				} else if(fch == 'o'){ // DO KEYWORD
					fch = getNextCharacter(fp, fo, fch);
					if(isspace(fch)){
						fileOutput(fo, "DO_KW");
						listAppend(tokenList, DO_KW, lineNumber);
						outputSpace(fo);
						return;
					} else if(isDelim(fch)){
						fileOutput(fo, "DO_KW");
						listAppend(tokenList, DO_KW, lineNumber);
						isDelimiter(fch, fp, fo);
						return;
					}
					if(fch == 'u'){ // DOUBLE KEYWORD
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'b'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'l'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'e'){
						fch = getNextCharacter(fp, fo, fch);
					if(isspace(fch)){
						fileOutput(fo, "DOUBLE_KW");
						listAppend(tokenList, DOUBLE_KW, lineNumber);
						outputSpace(fo);
						return;
					}}}}}}
			} else if(fch == 'e'){
				fch = getNextCharacter(fp, fo, fch);
				if(fch == 'l'){ // ELSE KEYWORD
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 's'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'e'){
					fch = getNextCharacter(fp, fo, fch);
				if(isspace(fch)){
					fileOutput(fo, "ELSE_KW");
					listAppend(tokenList, ELSE_KW, lineNumber);
					outputSpace(fo);
					return;
				} else if(isDelim(fch)){
					fileOutput(fo, "ELSE_KW");
					listAppend(tokenList, ELSE_KW, lineNumber);
					isDelimiter(fch, fp, fo);
					return;
				}}}
				} else if(fch == 'n'){ // ENUM KEYWORD
					fch = getNextCharacter(fp, fo, fch);
					if(fch == 'u'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'm'){
						fch = getNextCharacter(fp, fo, fch);
					if(isspace(fch)){
						fileOutput(fo, "ENUM_KW");
						listAppend(tokenList, ENUM_KW, lineNumber);
						outputSpace(fo);
						return;
					} else if(isDelim(fch)){
						fileOutput(fo, "ENUM_KW");
						listAppend(tokenList, ENUM_KW, lineNumber);
						isDelimiter(fch, fp, fo);
						return;
					}}}}
			} else if(fch == 'f'){
				fch = getNextCharacter(fp, fo, fch);
				if(fch == 'i'){ // FINAL KEYWORD
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'n'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'a'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'l'){
					fch = getNextCharacter(fp, fo, fch);
				if(isspace(fch)){
					fileOutput(fo, "FINAL_KW");
					listAppend(tokenList, FINAL_KW, lineNumber);
					outputSpace(fo);
					return;
				}}}}
				} else if(fch == 'l'){ // FLOAT KEYWORD
					fch = getNextCharacter(fp, fo, fch);
					if(fch == 'o'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'a'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 't'){
						fch = getNextCharacter(fp, fo, fch);
					if(isspace(fch)){
						fileOutput(fo, "FLOAT_KW");
						listAppend(tokenList, FLOAT_KW, lineNumber);
						outputSpace(fo);
						return;
					}}}}
				} else if(fch == 'o'){ // FOR KEYWORD
					fch = getNextCharacter(fp, fo, fch);
					if(fch == 'r'){
						fch = getNextCharacter(fp, fo, fch);
					if(isspace(fch)){
						fileOutput(fo, "FOR_KW");
						listAppend(tokenList, FOR_KW, lineNumber);
						outputSpace(fo);
						return;
					} else if(isDelim(fch)){
						fileOutput(fo, "FOR_KW");
						listAppend(tokenList, FOR_KW, lineNumber);
						isDelimiter(fch, fp, fo);
						return;
					}}
				} else if(fch == 'a'){ // FALSE RESERVE WORD
					fch = getNextCharacter(fp, fo, fch);
					if(fch == 'l'){
						fch = getNextCharacter(fp, fo, fch);
						if(fch == 's'){
							fch = getNextCharacter(fp, fo, fch);
						if(fch == 'e'){
							fch = getNextCharacter(fp, fo, fch);
						if(isspace(fch)){
							fileOutput(fo, "FALSE_RW");
							listAppend(tokenList, FALSE_RW, lineNumber);
							outputSpace(fo);
							return;
						} else if(isDelim(fch)){
							fileOutput(fo, "FALSE_RW");
							listAppend(tokenList, FALSE_RW, lineNumber);
							isDelimiter(fch, fp, fo);
							return;
						}}}}}
			} else if(fch == 'i'){
				fch = getNextCharacter(fp, fo, fch);
				if(fch == 'f'){ // IF KEYWORD
					fch = getNextCharacter(fp, fo, fch);
				if(isspace(fch)){
					fileOutput(fo, "IF_KW");
					listAppend(tokenList, IF_KW, lineNumber);
					outputSpace(fo);
					return;
				} else if(isDelim(fch)){
					fileOutput(fo, "IF_KW");
					listAppend(tokenList, IF_KW, lineNumber);
					isDelimiter(fch, fp, fo);
					return;
				}} else if(fch == 'n' && passedNotKeyword == 0){ // INT KEYWORD
					fch = getNextCharacter(fp, fo, fch);
					if(fch == 't'){
						fch = getNextCharacter(fp, fo, fch);
					if(isspace(fch)){
						fileOutput(fo, "INT_KW");
						listAppend(tokenList, INT_KW, lineNumber);
						outputSpace(fo);
						return;
					} else if(isDelim(fch)){
						fileOutput(fo, "INT_KW");
						listAppend(tokenList, INT_KW, lineNumber);
						isDelimiter(fch, fp, fo);
						return;
					}}
				} else if(fch == 'm'){ // IMPORT KEYWORD
					fch = getNextCharacter(fp, fo, fch);
					if(fch == 'p'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'o'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'r'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 't'){
						fch = getNextCharacter(fp, fo, fch);
					if(isspace(fch)){
						fileOutput(fo, "IMPORT_KW");
						listAppend(tokenList, IMPORT_KW, lineNumber);
						outputSpace(fo);
						return;
				}}}}}}
			} else if(fch == 'n'){
				fch = getNextCharacter(fp, fo, fch);
				if(fch == 'u'){ // NULL RESERVE WORD
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'l'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'l'){
					fch = getNextCharacter(fp, fo, fch);
				if(isspace(fch)){
					fileOutput(fo, "NULL_RW");
					listAppend(tokenList, NULL_RW, lineNumber);
					outputSpace(fo);
					return;
				} else if(isDelim(fch)){
					fileOutput(fo, "NULL_RW");
					listAppend(tokenList, NULL_RW, lineNumber);
					isDelimiter(fch, fp, fo);
					return;
				}}}}
			} else if(fch == 'r'){ 
				fch = getNextCharacter(fp, fo, fch);
				if(fch == 'e'){ // RETURN KEYWORD
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 't'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'u'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'r'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'n'){
					fch = getNextCharacter(fp, fo, fch);
				if(isspace(fch)){
					fileOutput(fo, "RETURN_KW");
					listAppend(tokenList, RETURN_KW, lineNumber);
					outputSpace(fo);
					return;
				} else if(isDelim(fch)){
					fileOutput(fo, "RETURN_KW");
					listAppend(tokenList, RETURN_KW, lineNumber);
					isDelimiter(fch, fp, fo);
					return;	
				}}}}} else if(fch == 'a'){ // READ FUNCTION
					fch = getNextCharacter(fp, fo, fch);
					if(fch == 'd'){
						fch = getNextCharacter(fp, fo, fch);
					if(isspace(fch)){
						fileOutput(fo, "READ_FN");
						listAppend(tokenList, READ_FN, lineNumber);
						outputSpace(fo);
						return;
					} else if(isDelim(fch)){
						fileOutput(fo, "READ_FN");
						listAppend(tokenList, READ_FN, lineNumber);
						isDelimiter(fch, fp, fo);
						return;	
				}}}} 
			} else if(fch == 's'){
				fch = getNextCharacter(fp, fo, fch);
				if(fch == 't'){ // STATIC KEYWORD
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'a'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 't'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'i'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'c'){
					fch = getNextCharacter(fp, fo, fch);
				if(isspace(fch)){
					fileOutput(fo, "STATIC_KW");
					listAppend(tokenList, STATIC_KW, lineNumber);
					outputSpace(fo);
					return;
				}}}}} else if(fch == 'r'){ // STRING KEYWORD
						fch = getNextCharacter(fp, fo, fch);
						if(fch == 'i'){
							fch = getNextCharacter(fp, fo, fch);
						if(fch == 'n'){
							fch = getNextCharacter(fp, fo, fch);
						if(fch == 'g'){
							fch = getNextCharacter(fp, fo, fch);
						if(isspace(fch)){
							fileOutput(fo, "STRING_KW");
							listAppend(tokenList, STRING_KW, lineNumber);
							outputSpace(fo);
							return;
					}}}}}
				} else if(fch == 'w'){ // SWITCH KEYWORD
					fch = getNextCharacter(fp, fo, fch);
					if(fch == 'i'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 't'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'c'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 'h'){
						fch = getNextCharacter(fp, fo, fch);
					if(isspace(fch)){
						fileOutput(fo, "SWITCH_KW");
						listAppend(tokenList, SWITCH_KW, lineNumber);
						outputSpace(fo);
						return;
					} else if(isDelim(fch)){
						fileOutput(fo, "SWITCH_KW");
						listAppend(tokenList, SWITCH_KW, lineNumber);
						isDelimiter(fch, fp, fo);
						return;
				}}}}}}
			} else if(fch == 't'){
				fch = getNextCharacter(fp, fo, fch);
				if(fch == 'r'){ // TRUE RESERVE WORD
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'u'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'e'){
					fch = getNextCharacter(fp, fo, fch);
				if(isspace(fch)){
					fileOutput(fo, "TRUE_RW");
					listAppend(tokenList, TRUE_RW, lineNumber);
					outputSpace(fo);
					return;
				} else if(isDelim(fch)){
					fileOutput(fo, "TRUE_RW");
					listAppend(tokenList, TRUE_RW, lineNumber);
					isDelimiter(fch, fp, fo);
					return;
				}}}}
			} else if(fch == 'v'){ // VOID KEYWORD
				fch = getNextCharacter(fp, fo, fch);
				if(fch == 'o'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'i'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'd'){
					fch = getNextCharacter(fp, fo, fch);
				if(isspace(fch)){
					fileOutput(fo, "VOID_KW");
					listAppend(tokenList, VOID_KW, lineNumber);
					outputSpace(fo);
					return;
				}}}}
			} else if(fch == 'w'){ // WHILE KEYWORD
				fch = getNextCharacter(fp, fo, fch);
				if(fch == 'h'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'i'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'l'){
					fch = getNextCharacter(fp, fo, fch);
				if(fch == 'e'){
					fch = getNextCharacter(fp, fo, fch);
				if(isspace(fch)){
					fileOutput(fo, "WHILE_KW");
					listAppend(tokenList, WHILE_KW, lineNumber);
					outputSpace(fo);
					return;
				} else if(isDelim(fch)){
					fileOutput(fo, "WHILE_KW");
					listAppend(tokenList, WHILE_KW, lineNumber);
					isDelimiter(fch, fp, fo);
					return;
				}}}}} else if(fch == 'r'){ // WRITE FUNCTION
					fch = getNextCharacter(fp, fo, fch);
					if(fch == 'i'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch == 't'){
						fch = getNextCharacter(fp, fo, fch);
					if(fch =='e'){
						fch = getNextCharacter(fp, fo, fch);
					if(isspace(fch)){
						fileOutput(fo, "WRITE_FN");
						listAppend(tokenList, WRITE_FN, lineNumber);
						outputSpace(fo);
						return;
					} else if(isDelim(fch)){
						fileOutput(fo, "WRITE_FN");
						listAppend(tokenList, WRITE_FN, lineNumber);
						isDelimiter(fch, fp, fo);
						return;
					} else if(fch == 'l'){
						fch = getNextCharacter(fp, fo, fch);
						if(fch == 'n'){
							fch = getNextCharacter(fp, fo, fch);
						if(isspace(fch)){
							fileOutput(fo, "WRITELN_FN");
							listAppend(tokenList, WRITELN_FN, lineNumber);
							outputSpace(fo);
							return;
						} else if(isDelim(fch)){
							fileOutput(fo, "WRITELN_FN");
							listAppend(tokenList, WRITELN_FN, lineNumber);
							isDelimiter(fch, fp, fo);
							return;
				}}}}}}}
			} else{
				if(isalpha(fch) || fch == '_' || isdigit(fch))
					validIden = 1;
				else
					encounteredSpecialSymbols = 1;
				fch = getNextCharacter(fp, fo, fch);
				passedNotKeyword = 1;
			}
			if(isspace(fch))
				break;
			if(isOper(fch))
				break;
			if(isDelim(fch))
				break;
		}
		if(validIden == 1 && encounteredSpecialSymbols == 0){
			fileOutput(fo, "VALID_IDEN");
			listAppend(tokenList, VALID_IDEN, lineNumber);
		} else
			fileOutput(fo, "INVALID_IDEN");
		if(isOper(fch))
			isOperator(fch, fp, fo);
		if(isDelim(fch))
			isDelimiter(fch, fp, fo);
	}
}

int isOper(char fch){
	int intCounter;
	for(intCounter = 0; intCounter < sizeof(arrOperators); intCounter++){
		if(fch == arrOperators[intCounter])
			return 1;
	}
	return 0;
}

int isDelim(char fch){
	int intCounter;
	for(intCounter = 0; intCounter < sizeof(arrDelimiters); intCounter++){
		if(fch == arrDelimiters[intCounter])
			return 1;
	}
	return 0;
}

void getLexemes(FILE *fo){
	Stack * stack = tokenList->first;
//	char line[255];
//	int counter = 0;
//	while(fgets(line, sizeof(line), fo) != NULL){
//		char * lexeme = strtok(line, " |");
//		strcpy(stack->lexeme, lexeme);
////        printf("%s\n", stack->lexeme);
//        stack = stack->next;
//        counter++;
//	}
	char line[1024];
    while (fgets(line, 1024, fo)){
        char* tmp = strdup(line);
//        printf("Field 1 would be %s\n", getfield(tmp, 1));
        // NOTE strtok clobbers tmp
        strcpy(stack->lexeme, getfield(tmp, 1));
        stack = stack->next;
        free(tmp);
    }
}

const char * getfield(char * line, int num){
    const char* tok;
    for (tok = strtok(line, "|");
            tok && *tok;
            tok = strtok(NULL, "|\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}
