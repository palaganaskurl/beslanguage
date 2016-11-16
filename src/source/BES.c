//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <conio.h>
//#include "..\headers\LINKEDLIST.H"
//#include "..\headers\LEXER.H"
//
///* 	Change Log
// 	08/03/2016 - Added int passedNotKeyword. For bug like -> int print; print is determined to be INT_KW.
//	08/03/2016 - Created function for file printing for shorter code.
// 	08/03/2016 - Decimal literal invalid solved.
// 	08/03/2016 - Remove unnecessary TokenTypeBesBesBesBesBesBesBesBess.
// 	08/03/2016 - Added int encounteredSpecialSymbols. For determining invalid identifiers.
// 	08/03/2016 - Added ^ for arithmetic operations
// 	08/03/2016 - Added method to determine noise words. Not sure if correct. Noise words example : $, @ 
//				 Symbols not in out character sets
// 	08/08/2016 - Added line number for tokens
// 	08/18/2016 - Added hashtable
// 	08/19/2016 - Added semantics for main data types. 
// 				 Assignment incompatibility. 
//				 Variables not declared and already declared.
//	08/19/2016 - Added semantics in conditions
//	08/19/2016 - Added break statement. Fuck ba't wala nun. Hahaha.	
//	08/22/2016 - Added variable scopes		 
//	09/01/2016 - Added warning variable not used. Hindi pa tapos.
//*/
//int checkFileExtension(char[]);
//
//int checkFileExtension(char fileName[]){
//	char *ext = strlwr(strchr(fileName, '.'));
//	if(strcmp(ext + 1, "bes") == 0)
//		return 1;
//	else
//		return 0;
//}
//
//int main(){
//	FILE *fp; // file input
//	FILE *fo; // file output
//	char fileName[100]; // filename
//	TokenList * tokenList;
//
//	printf("Enter the name of bes file: ");
//	scanf("%[^\n]s", fileName);
//
//	fp = fopen(fileName, "r");
//	fo = fopen("SYMBOL TABLE.txt", "w");
//	
//	if(fp == NULL){
//		printf("\nFile doesn't exists!");
//		return 0;	
//	}
//	
//	// note to self add ^ exponent lexer
//	
//	if(!checkFileExtension(fileName)){
//		printf("\nNot a BES file!");
//		return 0;
//	}
//	
//	checkFileExtension(fileName);
//	tokenList = createTokenList();
//	
//	initLexer(fp, fo);
//	
//	initToken(tokenList);
//	readToken();
//
//	getch();
//	return 0;
//}
