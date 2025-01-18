#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "translator.h"




// таблиц€ лексем
Token* TokenTable;
// к≥льк≥сть лексем
unsigned int TokensNum;

// таблиц€ ≥дентиф≥катор≥в
Id* IdTable;
// таблиц€ м≥ток
Label* LabelTable;
// к≥льк≥сть ≥дентиф≥катор≥в та м≥ток
unsigned int IdNum, LabelNum;

int main(int argc, char* argv[])
{
	// вид≥ленн€ пам'€т≥ п≥д таблицю лесем
	TokenTable = new Token[MAX_TOKENS];

	// вид≥ленн€ пам'€т≥ п≥д таблицю ≥дентиф≥катор≥в
	IdTable = new Id[MAX_IDENTIFIER];
	LabelTable = new Label[100];

	char InputFile[32] = "";

	FILE* InFile;

	if (argc != 2)
	{
		printf("Input file name: ");
		gets_s(InputFile);
	}
	else
	{
		strcpy_s(InputFile, argv[1]);
	}

	if ((fopen_s(&InFile, InputFile, "rt")) != 0)
	{
		printf("Error: Can not open file: %s\n", InputFile);
		return 1;
	}

	char NameFile[32] = "";
	int i = 0;
	while (InputFile[i] != '.' && InputFile[i] != '\0')
	{
		NameFile[i] = InputFile[i];
		i++;
	}
	if(strcmp(&InputFile[i], ".k10"))
	{
		printf("Error: File extension has to be .k10\n");
		return 1;
	}
	NameFile[i] = '\0';

	char TokenFile[32];
	strcpy_s(TokenFile, NameFile);
	strcat_s(TokenFile, ".token");

	// лексичний анал≥з
	TokensNum = GetTokens(InFile, TokenTable);

	PrintTokensToFile(TokenFile, TokenTable, TokensNum);
	fclose(InFile);

	printf("\nLexical analysis completed: %d tokens. List of tokens in the file %s\n", TokensNum, TokenFile);
	//PrintTokens(TokenTable, TokensNum);

	// синтаксичний анал≥з
	Parser();

	// створенн€ абстрактного синтаксичного дерева
	ASTNode* ASTree = ParserAST();

	//printf("\nAbstract Syntax Tree:\n");
	//PrintAST(ASTree, 0);

	char AST[32];
	strcpy_s(AST, NameFile);
	strcat_s(AST, ".ast");
	// Open output file
	FILE* ASTFile;
	fopen_s(&ASTFile, AST, "w");
	if (!ASTFile)
	{
		printf("Failed to open output file.\n");
		exit(1);
	}
	PrintASTToFile(ASTree, 0, ASTFile);
	printf("\nAST has been created and written to %s.\n", AST);

	char OutputFile[32];
	strcpy_s(OutputFile, NameFile);
	strcat_s(OutputFile, ".c");
    
	FILE* outFile;
	fopen_s(&outFile, OutputFile, "w");
	if (!outFile)
	{
		printf("Failed to open output file.\n");
		exit(1);
	}
	// генерац≥€ вих≥дного — коду
	generateCCode(outFile);
	printf("\nC code has been generated and written to %s.\n", OutputFile);

 
	char OutputFileFromAST[32];
	strcpy_s(OutputFileFromAST, NameFile);
	strcat_s(OutputFileFromAST, "_fromAST.c");
	// Open output file
	FILE* outFileFromAST;
	fopen_s(&outFileFromAST, OutputFileFromAST, "w");
	if (!outFileFromAST)
	{
		printf("Failed to open output file.\n");
		exit(1);
	}
	// Generate C code from AST
	generateCodefromAST(ASTree, outFileFromAST);
	// генерац≥€ вих≥дного — коду
	printf("\nC code has been generated and written to %s.\n", OutputFileFromAST);

	// Close the file
	_fcloseall();

	destroyTree(ASTree);

	delete[]TokenTable;
	delete[]IdTable;
	delete[]LabelTable;

	char setVar[256] = "\"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Auxiliary\\Build\\vcvars64.bat\"";

	char createExe[128];
	sprintf_s(createExe, "cl %s", OutputFileFromAST);
	strcat_s(setVar, " && ");
	strcat_s(setVar, createExe);
	system(setVar);
	system("pause");

	return 0;
}