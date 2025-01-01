#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "translator.h"

// функція отримує лексеми з вхідного файлу F і записує їх у таблицю лексем TokenTable 
// результат функції - кількість лексем
unsigned int GetTokens(FILE* F, Token TokenTable[])
{
	States state = Start;
	Token TempToken;
	// кількість лексем
	unsigned int NumberOfTokens = 0;
	char ch, buf[16];
	int line = 1;
	// читання першого символу з файлу
	ch = getc(F);
	// пошук лексем
	while (1)
	{
		switch (state)
		{
		case Start:
		{
			if (ch == EOF)
				state = EndOfFile;
			else
				if (ch <= 'z' && ch >= 'a')
					state = Letter;
				else
					if (ch <= '9' && ch >= '0')
						state = Digit;
					else
						if (ch == ' ' || ch == '\t' || ch == '\n')
							state = Separators;
						else
							if (ch == '/')
								state = SComment;
							else
								state = Another;
			break;
		}
		// стан Finish - кінець виділення чергової лексеми і запис лексеми у таблицю лексем
		case Finish:
		{
			if (NumberOfTokens < MAX_TOKENS)
			{
				TokenTable[NumberOfTokens++] = TempToken;
				if (ch != EOF)
					state = Start;
				else
					state = EndOfFile;
			}
			else
			{
				printf("\n\t\t\ttoo many tokens !!!\n");
				return NumberOfTokens - 1;
			}
			break;
		}
		// стан EndOfFile - кінець файлу, можна завершувати пошук лексем
		case EndOfFile:
		{
			return NumberOfTokens;
		}
		// стан Letter - поточний символ - маленька літера, поточна лексема - ключове слово або ідентифікатор

		case Letter:
		{
			buf[0] = ch;
			int j = 1;
			ch = getc(F);
			while (((ch <= 'z' && ch >= 'A') || (ch <= '9' && ch >= '0')) && j < 15)
			{
				buf[j++] = ch;
				ch = getc(F);
			}
			buf[j] = '\0';
			TypeOfTokens temp_type = Unknown;
			if (!strcmp(buf, "start"))
				temp_type = StartProgram;
			else
				if (!strcmp(buf, "var"))
					temp_type = Variable;
				else
					if (!strcmp(buf, "integer16"))
						temp_type = Type;
					else
						if (!strcmp(buf, "finish"))
							temp_type = EndProgram;
						else
							if (!strcmp(buf, "scan"))
								temp_type = Input;
							else
								if (!strcmp(buf, "print"))
									temp_type = Output;
								else
									if (!strcmp(buf, "if"))
										temp_type = If;
									else
										if (!strcmp(buf, "for"))
											temp_type = For;
										else
											if (!strcmp(buf, "to"))
												temp_type = To;
											else
												if (!strcmp(buf, "downto"))
													temp_type = Downto;
												else
													if (!strcmp(buf, "do"))
														temp_type = Do;
													else
														if (!strcmp(buf, "while"))
															temp_type = While;
														else
															if (!strcmp(buf, "end"))
																temp_type = End;
															else
																if (!strcmp(buf, "repeat"))
																	temp_type = Repeat;
																else
																	if (!strcmp(buf, "until"))
																		temp_type = Until;
																	else
																		if (!strcmp(buf, "goto"))
																			temp_type = Goto;
																		else
																			if (!strcmp(buf, "add"))
																				temp_type = Add;
																			else
																				if (!strcmp(buf, "sub"))
																					temp_type = Sub;
																				else
																					if (!strcmp(buf, "and"))
																						temp_type = And;
																					else
																						if (!strcmp(buf, "or"))
																							temp_type = Or;
																						else
																							if (!strcmp(buf, "not"))
																								temp_type = Not;
																							else
																								if (!strcmp(buf, "continue"))
																									temp_type = Continue;
																								else
																									if (!strcmp(buf, "break"))
																										temp_type = Break;
																									else
																										if (!strcmp(buf, "exit"))
																											temp_type = Exit;
																										else
																											if (!strcmp(buf, "else"))
																												temp_type = Else;
																											else
																												if (!strcmp(buf, "program"))
																													temp_type = ProgramName;
																												else
																													if (strlen(buf) == 10 && buf[0] >= 'a') {
																														temp_type = Identifier;
																														for (int i = 1; i < 10; i++) {
																															if (buf[i] >= 'a') {
																																temp_type = Unknown;
																															}
																														}
																													}
			strcpy_s(TempToken.name, buf);
			TempToken.type = temp_type;
			TempToken.value = 0;
			TempToken.line = line;
			state = Finish;
			break;
		}
		// стан Digit - поточний символ - цифра, поточна лексема - число
		case Digit:
		{
			buf[0] = ch;
			int j = 1;
			ch = getc(F);
			while ((ch <= '9' && ch >= '0') && j < 15)
			{
				buf[j++] = ch;
				ch = getc(F);
			}
			buf[j] = '\0';
			if (buf[0] == '-' && buf[1] == '\0') {
				TempToken.name[0] = '-';
				TempToken.name[1] = '\0';
				TempToken.type = Unknown;
				TempToken.value = 0;
				TempToken.line = line;
				state = Finish;
				break;
			}
			strcpy_s(TempToken.name, buf);
			TempToken.type = Number;
			TempToken.value = atoi(buf);
			TempToken.line = line;
			state = Finish;
			break;
		}
		// стан Separators - поточний символ пробіл, символ табуляції або переходу на новий рядок - видаляємо їх

		case Separators:
		{
			if (ch == '\n')
				line++;
			ch = getc(F);
			state = Start;
			break;
		}
		// стан SComment - поточний символ /, можливо це коментар
		case SComment:
		{
			ch = getc(F);
			if (ch == '*')
				state = Comment;
			else
			{
				strcpy_s(TempToken.name, "/");
				TempToken.type = Div;
				TempToken.value = 0;
				TempToken.line = line;
				state = Finish;
			}
			break;
		}
		// стан Comment - поточний символ /, отже це коментар, видаляємо усі символи до кінця рядка

		case Comment:
		{
			while (ch != EOF)
			{
				ch = getc(F);
				if (ch == '*') {
					ch = getc(F);
					if (ch == '/' || ch == EOF) {
						break;
					}
				}
			}
			if (ch == EOF)
			{
				state = EndOfFile;
				break;
			}
			//line++;
			ch = getc(F);
			state = Start;
			break;
		}
		// стан Another - поточний символ - символ, відмінний від попередніх
		case Another:
		{
			switch (ch)
			{
			case '(':
			{
				strcpy_s(TempToken.name, "(");
				TempToken.type = LBraket;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}
			case ')':
			{
				strcpy_s(TempToken.name, ")");
				TempToken.type = RBraket;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}
			case ';':
			{
				strcpy_s(TempToken.name, ";");
				TempToken.type = Semicolon;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}
			case ',':
			{
				strcpy_s(TempToken.name, ",");
				TempToken.type = Comma;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}
			case '*':
			{
				strcpy_s(TempToken.name, "*");
				TempToken.type = Mul;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}
			case '<':
			{
				ch = getc(F);
				if (ch == '>')
				{
					strcpy_s(TempToken.name, "<>");
					TempToken.type = NotEquality;
					TempToken.value = 0;
					TempToken.line = line;
					ch = getc(F);
					state = Finish;
				}
				else
				{
					strcpy_s(TempToken.name, "<");
					TempToken.type = Less;
					TempToken.value = 0;
					TempToken.line = line;
					state = Finish;
				}
				break;
			}
			case '>':
			{
				strcpy_s(TempToken.name, ">");
				TempToken.type = Greate;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}
			case'-':
			{
				state = Digit;
				break;
			}
			case '=':
			{
				ch = getc(F);
				if (ch == '=')
				{
					ch = getc(F);
					if (ch == '>')
					{
						strcpy_s(TempToken.name, "==>");
						TempToken.type = Assign;
						TempToken.value = 0;
						TempToken.line = line;
						ch = getc(F);
						state = Finish;
					}
					else
					{
						TempToken.name[0] = '=';
						TempToken.name[1] = '=';
						TempToken.name[2] = '\0';
						TempToken.type = Unknown;
						TempToken.value = 0;
						TempToken.line = line;
						state = Finish;
					}
				}
				else
				{
					strcpy_s(TempToken.name, "=");
					TempToken.type = Equality;
					TempToken.value = 0;
					TempToken.line = line;
					state = Finish;

				}
				break;
			}
			case ':':
			{
				strcpy_s(TempToken.name, ":");
				TempToken.type = Colon;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}
			case '%':
			{
				strcpy_s(TempToken.name, "%");
				TempToken.type = Mod;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}
			default:
			{
				TempToken.name[0] = ch;
				TempToken.name[1] = '\0';
				TempToken.type = Unknown;
				TempToken.value = 0;
				TempToken.line = line;
				ch = getc(F);
				state = Finish;
				break;
			}
			}
		}
		}
	}
}


void PrintTokens(Token TokenTable[], unsigned int TokensNum)
{
	char type_tokens[16];
	printf("\n\n---------------------------------------------------------------------------\n");
	printf("| TOKEN TABLE |\n");
	printf("---------------------------------------------------------------------------\n");
	printf("| line number | token | value | token code | type of token |\n");
	printf("---------------------------------------------------------------------------");
	for (unsigned int i = 0; i < TokensNum; i++)
	{
		switch (TokenTable[i].type)
		{
		case Continue:
			strcpy_s(type_tokens, "Continue");
			break;
		case Break:
			strcpy_s(type_tokens, "Break");
			break;
		case For:
			strcpy_s(type_tokens, "For");
			break;
		case To:
			strcpy_s(type_tokens, "To");
			break;
		case Do:
			strcpy_s(type_tokens, "Do");
			break;
		case Downto:
			strcpy_s(type_tokens, "Downto");
			break;
		case While:
			strcpy_s(type_tokens, "While");
			break;
		case End:
			strcpy_s(type_tokens, "End");
			break;
		case Exit:
			strcpy_s(type_tokens, "Exit");
			break;
		case Repeat:
			strcpy_s(type_tokens, "Repeat");
			break;
		case Until:
			strcpy_s(type_tokens, "Until");
			break;
		case Goto:
			strcpy_s(type_tokens, "Goto");
			break;
		case Colon:
			strcpy_s(type_tokens, "Colon");
			break;
		case ProgramName:
			strcpy_s(type_tokens, "ProgramName");
			break;
		case StartProgram:
			strcpy_s(type_tokens, "StartProgram");
			break;
		case Variable:
			strcpy_s(type_tokens, "Variable");
			break;
		case Type:
			strcpy_s(type_tokens, "Integer16");
			break;
		case Identifier:
			strcpy_s(type_tokens, "Identifier");
			break;
		case EndProgram:
			strcpy_s(type_tokens, "EndProgram");
			break;
		case Input:
			strcpy_s(type_tokens, "Input");
			break;
		case Output:
			strcpy_s(type_tokens, "Output");
			break;
		case If:
			strcpy_s(type_tokens, "If");
			break;
		case Else:
			strcpy_s(type_tokens, "Else");
			break;
		case Assign:
			strcpy_s(type_tokens, "Assign");
			break;
		case Add:
			strcpy_s(type_tokens, "Add");
			break;
		case Sub:
			strcpy_s(type_tokens, "Sub");
			break;
		case Mul:
			strcpy_s(type_tokens, "Mul");
			break;
		case Div:
			strcpy_s(type_tokens, "Div");
			break;
		case Mod:
			strcpy_s(type_tokens, "Mod");
			break;
		case Equality:
			strcpy_s(type_tokens, "Equality");
			break;
		case NotEquality:
			strcpy_s(type_tokens, "NotEquality");
			break;
		case Greate:
			strcpy_s(type_tokens, "Greate");
			break;
		case Less:
			strcpy_s(type_tokens, "Less");
			break;
		case Not:
			strcpy_s(type_tokens, "Not");
			break;
		case And:
			strcpy_s(type_tokens, "And");
			break;
		case Or:
			strcpy_s(type_tokens, "Or");
			break;
		case LBraket:
			strcpy_s(type_tokens, "LBraket");
			break;
		case RBraket:
			strcpy_s(type_tokens, "RBraket");
			break;
		case Number:
			strcpy_s(type_tokens, "Number");
			break;
		case Semicolon:
			strcpy_s(type_tokens, "Semicolon");
			break;
		case Comma:
			strcpy_s(type_tokens, "Comma");
			break;
		case Unknown:
			strcpy_s(type_tokens, "Unknown");
			break;
		}
		printf("\n|%12d |%16s |%11d |%11d | %-13s |\n",
			TokenTable[i].line,
			TokenTable[i].name,
			TokenTable[i].value,
			TokenTable[i].type,
			type_tokens);
		printf("---------------------------------------------------------------------------");
	}
}

// функція друкує таблицю лексем у файл
void PrintTokensToFile(char* FileName, Token TokenTable[], unsigned int TokensNum)
{
	FILE* F;
	if ((fopen_s(&F, FileName, "wt")) != 0)
	{
		printf("Error: Can not create file: %s\n", FileName);
		return;
	}
	char type_tokens[16];
	fprintf(F, "---------------------------------------------------------------------------\n");
	fprintf(F, "| TOKEN TABLE |\n");
	fprintf(F, "---------------------------------------------------------------------------\n");
	fprintf(F, "| line number | token | value | token code | type of token |\n");
	fprintf(F, "---------------------------------------------------------------------------");
	for (unsigned int i = 0; i < TokensNum; i++)
	{
		switch (TokenTable[i].type)
		{
		case Exit:
			strcpy_s(type_tokens, "Exit");
			break;
		case For:
			strcpy_s(type_tokens, "For");
			break;
		case To:
			strcpy_s(type_tokens, "To");
			break;
		case Do:
			strcpy_s(type_tokens, "Do");
			break;
		case Downto:
			strcpy_s(type_tokens, "Downto");
			break;
		case While:
			strcpy_s(type_tokens, "While");
			break;
		case End:
			strcpy_s(type_tokens, "End");
			break;
		case Repeat:
			strcpy_s(type_tokens, "Repeat");
			break;
		case Until:
			strcpy_s(type_tokens, "Until");
			break;
		case Goto:
			strcpy_s(type_tokens, "Goto");
			break;
		case Colon:
			strcpy_s(type_tokens, "Colon");
			break;
		case ProgramName:
			strcpy_s(type_tokens, "ProgramName");
			break;
		case StartProgram:
			strcpy_s(type_tokens, "StartProgram");
			break;
		case Variable:
			strcpy_s(type_tokens, "Variable");
			break;
		case Identifier:
			strcpy_s(type_tokens, "Identifier");
			break;
		case Type:
			strcpy_s(type_tokens, "Integer16");
			break;
		case EndProgram:
			strcpy_s(type_tokens, "EndProgram");
			break;
		case Input:
			strcpy_s(type_tokens, "Input");
			break;
		case Output:
			strcpy_s(type_tokens, "Output");
			break;
		case If:
			strcpy_s(type_tokens, "If");
			break;
		case Else:
			strcpy_s(type_tokens, "Else");
			break;
		case Assign:
			strcpy_s(type_tokens, "Assign");
			break;
		case Add:
			strcpy_s(type_tokens, "Add");
			break;
		case Sub:
			strcpy_s(type_tokens, "Sub");
			break;
		case Mul:
			strcpy_s(type_tokens, "Mul");
			break;
		case Div:
			strcpy_s(type_tokens, "Div");
			break;
		case Mod:
			strcpy_s(type_tokens, "Mod");
			break;
		case Equality:
			strcpy_s(type_tokens, "Equality");
			break;
		case NotEquality:
			strcpy_s(type_tokens, "NotEquality");
			break;
		case Greate:
			strcpy_s(type_tokens, "Greate");
			break;
		case Less:
			strcpy_s(type_tokens, "Less");
			break;
		case Not:
			strcpy_s(type_tokens, "Not");
			break;
		case And:
			strcpy_s(type_tokens, "And");
			break;
		case Or:
			strcpy_s(type_tokens, "Or");
			break;
		case LBraket:
			strcpy_s(type_tokens, "LBraket");
			break;
		case RBraket:
			strcpy_s(type_tokens, "RBraket");
			break;
		case Number:
			strcpy_s(type_tokens, "Number");
			break;
		case Semicolon:
			strcpy_s(type_tokens, "Semicolon");
			break;
		case Comma:
			strcpy_s(type_tokens, "Comma");
			break;
		case Unknown:
			strcpy_s(type_tokens, "Unknown");
			break;
		}
		fprintf(F, "\n|%12d |%16s |%11d |%11d | %-13s |\n",
			TokenTable[i].line,
			TokenTable[i].name,
			TokenTable[i].value,
			TokenTable[i].type,
			type_tokens);
		fprintf(F, "---------------------------------------------------------------------------");
	}
	fclose(F);
}