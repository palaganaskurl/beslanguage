#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#define MAX_ARRAY 30

char arrKeywords[][MAX_ARRAY] = {
									"import",
									"int"
								};
char arrOperators[MAX_ARRAY] = {
									'+',
									'-',
									'*',
									'/',
									'%',
									'<',
									'>',
									'!',
									'|',
									'&',
									'='
							   };
char arrDelimiters[MAX_ARRAY] = {
									',',
									'{',
									'}',
									'[',
									']',
									';',
									'(',
									')'
								};
char formString[MAX_ARRAY]; // identifier or keyword
int formStringCounter = 0; // formString current element

int isOperator(char, FILE *);
int isIdentifier(char, FILE *);
int isDelimiter(char, FILE *);
int isNumberLiteral(char, FILE *);
int isSpaceFn(char, FILE *);
int isNewLine(char, FILE *);

int isKeyword(char *);
int isOper(char);
int isDelim(char);

int curPos = 0;

int isOperator(char fch, FILE *fp){
	char lch;
	if(isOper(fch)){
		printf("%c", fch);
		lch = fch;
		fch = fgetc(fp);
		if(!isOper(fch)){
			if(lch == '=')
				printf("\tASS_OPER\n");
			else if(lch == '+' || lch == '-' || lch == '*' || lch == '/' || lch == '%')
				printf("\tARITH_OPER\n");
			if(isspace(fch))
				printf("%c\tWHTSPC\n", fch);
			if(isdigit(fch))
				isNumberLiteral(fch,fp);
			return 1;
		}
		if((lch == '&' && fch == '&') || (lch == '|' && fch == '|') || (fch == '!'))
			printf("%c\tLOG_OPER\n", fch);
		else if((lch == '<' && fch == '=') || (lch == '>' && fch == '=') || (lch == '!' && fch == '=') || (fch == '<') || (fch == '>'))
			printf("%c\tREL_OPER\n", fch);
		else if((lch == '+' && fch == '+') || (lch == '-' && fch == '-'))
			printf("%c\tUNA_OPER\n", fch);
		else if(isdigit(fch))
			isNumberLiteral(fch,fp);
		return 1;
	} else 
		return 0;
}

int isSpaceFn(char fch, FILE *fp){
	if(isspace(fch)){
		printf("%c\tWHTSPC\n", fch);
		return 1;
	} else	
		return 0;
}

int isNewLine(char fch, FILE *fp){
	if(fch == '\n'){
		printf("%c\tNEWLINE\n", fch);
		return 1;
	} else
		return 0;
		
}

int isNumberLiteral(char fch, FILE *fp){
	int decCount = 0;
	if(isdigit(fch)){
//		printf("%c\tnumlit\n", fch);
		while(1){
			if(isspace(fch) || isDelim(fch) || isOper(fch)){
				if(isOper(fch))
					isOperator(fch,fp);
				if(isDelim(fch))
					isDelimiter(fch, fp);
				break;	
			}
			printf("%c\tNUM_LIT\n", fch);	
			fch = fgetc(fp);
		}
		return 1;
	} else 
		return 0;
}

int isDelimiter(char fch, FILE *fp){
	if(isDelim(fch)){
		printf("%c\tDELIM\n", fch);
		curPos++;
		return 1;	
	} else 
		return 0;
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

int isIdentifier(char fch, FILE *fp){
	if(isalpha(fch)){
		while(1){
			if(isspace(fch) || isDelim(fch) || isOper(fch))
				break;
			formString[formStringCounter] = fch;
			formStringCounter++;
			if(isKeyword(formString)){
				fch = fgetc(fp);
				if(isspace(fch))
					printf("%c\tWHTSPC\n", fch);
				return 0;
			}
			fch = fgetc(fp);
		}
		printf("%s\tIDEN\n", formString);
		formStringCounter = 0;
		memset(formString, 0, sizeof(formString));
		if(isspace(fch))
			printf("%c\tWHTSPC\n", fch);
		if(isDelim(fch))
			printf("%c\tDELIM\n", fch);
		if(isOper(fch)) // unary operator
			isOperator(fch, fp);
		return 1;
	} else 
		return 0;
}

int isKeyword(char *str){
	int intCounter;
	for(intCounter = 0; intCounter < sizeof(arrKeywords); intCounter++){
		if(strcmp(str, arrKeywords[intCounter]) == 0){
			printf("%s\tKEYW\n", formString);
			formStringCounter = 0;
			memset(formString, 0, sizeof(formString));
			return 1;
		}
	}
	return 0;
}

char isNewLine(char fch){
	if(fch == '\n')
		printf("nl\n");
	else
		printf("nnl\n");
}

char isHeader(char fch){
	while(fch != ';'){
//		if(fch == )
	}
}
