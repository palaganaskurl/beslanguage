#include <stdio.h>
#include <conio.h>
#include "lexer.h"


int main(){
	
	FILE *fp; // file input
	char fch; // each character in file
	char ret; // return value
	
	fp = fopen("BES.TXT", "r");
	
	while((fch = fgetc(fp)) != EOF){
			isIdentifier(fch, fp);
			isDelimiter(fch, fp);
			isOperator(fch, fp);
			isNumberLiteral(fch, fp);
//			isSpaceFn(fch, fp);
//			isNewLine(fch, fp);
	}
	
	getch();
	return 0;	
}
