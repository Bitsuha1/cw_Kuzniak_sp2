#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "translator.h"

// таблиц€ лексем
extern Token* TokenTable;
// к≥льк≥сть лексем
extern unsigned int TokensNum;

// таблиц€ ≥дентиф≥катор≥в
extern Id* IdTable;
// таблиц€ м≥ток
extern Label* LabelTable;
// к≥льк≥сть ≥дентиф≥катор≥в
extern unsigned int IdNum;

static unsigned int inside_while = 0;
static unsigned int inside_for_repeat = 0;

static int pos = 0;

// наб≥р функц≥й дл€ рекурсивного спуску 
// на кожне правило - окрема функц≥€
void program();
void variable_declaration();
void variable_list();
void program_body();
void statement();
void assignment();
void arithmetic_expression();
void term();
void factor();
void input();
void output();
void conditional();
void logical_expression();
void and_expression();
void comparison();
void compound_statement();
void goto_statement();
void goto_mark();
void while_statement();
void continue_while();
void while_body();
void exit_while();
void for_loop();
void break_mark();
void repeat_body();
void repeat_loop();
void if_for_body();

// функц≥€ записуЇ оголошен≥ ≥дентиф≥катори в таблицю ≥дентиф≥катор≥в IdTable
// повертаЇ к≥льк≥сть ≥дентиф≥катор≥в
// перев≥р€Ї чи ус≥ використан≥ ≥дентиф≥катори оголошен≥
unsigned int IdIdentification(Id IdTable[], Token TokenTable[], unsigned int tokenCount);
// функц≥€ записуЇ оголошен≥ м≥тки в таблицю м≥ток LabelTable
// повертаЇ к≥льк≥сть м≥ток
// перев≥р€Ї чи ус≥ адресован≥ м≥тки оголошен≥
unsigned int LabelIdentification(Label LabelTable[], Token TokenTable[], unsigned int tokenCount);
// перев≥р€Ї, чи не виникаЇ конфл≥кту ≥мен м≥ж м≥тками та ≥дентиф≥каторами
void IdLabelCollision(Id IdTable[], Label LabelTable[], unsigned int ids, unsigned int labels);

// синтаксичний анал≥з методом рекурсивного спуску
// вх≥дн≥ дан≥ - глобальна таблиц€ лексем TokenTable ≥ таблиц€ м≥ток LabelTable
void Parser()
{
    program();
    printf("\nThe program is syntax correct.\n");
    unsigned int ids = IdIdentification(IdTable, TokenTable, TokensNum);
    printf("\n%d identifier found.\n", ids);
    unsigned int labels = LabelIdentification(LabelTable, TokenTable, TokensNum);
    printf("\n%d label found.\n", labels);
    IdLabelCollision(IdTable, LabelTable, ids, labels);

}

void match(TypeOfTokens expectedType)
{
    if (TokenTable[pos].type == expectedType)
        pos++;
    else
    {
        printf("\nSyntax error in line %d with name %s, expected %d: another type of lexeme was expected.\n", TokenTable[pos].line, TokenTable[pos].name, expectedType);
        exit(1);
    }
}

// <програма> ::= 'start' 'var' <оголошенн€ зм≥нних> ';' <т≥ло програми> 'stop'
void program()
{
    match(ProgramName);
    match(Identifier);
    match(Semicolon);
    match(StartProgram);
    match(Variable);
    variable_declaration();
    match(Semicolon);
    program_body();
    match(EndProgram);
}

// <оголошенн€ зм≥нних> ::= [<тип даних> <список зм≥нних>]
void variable_declaration()
{
    if (TokenTable[pos].type == Type)
    {
        pos++;
        variable_list();
    }
}

// <список зм≥нних> ::= <≥дентиф≥катор> { ',' <≥дентиф≥катор> }
void variable_list()
{
    match(Identifier);
    while (TokenTable[pos].type == Comma)
    {
        pos++;
        match(Identifier);
    }
}

// <т≥ло програми> ::= <оператор> ';' { <оператор> ';' }
void program_body()
{
    do
    {
        statement();
        //match(Semicolon);
    } while (TokenTable[pos].type != EndProgram);
}

void while_body()
{
    inside_while++;
    do
    {
        statement();
        //match(Semicolon);
    } while (TokenTable[pos].type != End);
    inside_while--;
}

void if_for_body() {
    while (TokenTable[pos].type != Semicolon) {
        statement();
    }
}

void repeat_body()
{
    inside_for_repeat++;
    do
    {
        statement();
    } while (TokenTable[pos].type != Until);
    inside_for_repeat--;
}

// <оператор> ::= <присвоЇнн€> | <вв≥д> | <вив≥д> | <умовний оператор> | <складений оператор>
void statement()
{
    switch (TokenTable[pos].type)
    {
    case Input: input(); break;
    case Output: output(); break;
    case If: conditional(); break;
    //case StartProgram: compound_statement(); break;
    case Continue: continue_while(); break;
    case Goto: goto_statement(); break;
    case Exit: exit_while(); break;
    case While: while_statement(); break;
    case Break: break_mark(); break;
    case For: for_loop(); break;
    case Repeat: repeat_loop(); break;
    default: { 
        if (TokenTable[pos + 1].type == Colon){
            goto_mark(); 
            break;
        }
        assignment(); 
        }
    }
}

void repeat_loop() {
    match(Repeat);
    repeat_body();
    match(Until);
    logical_expression();
}

void for_loop() {
    match(For);
    assignment();
    if (TokenTable[pos].type == Downto) {
        pos++;
    }
    else { match(To); }
    arithmetic_expression();
    match(Do);
    inside_for_repeat++;
    if_for_body();
    match(Semicolon);
    inside_for_repeat--;
}

void while_statement() {
    match(While);
    logical_expression();
    match(Do);
    while_body();
    match(End);
    match(While);
}

void continue_while() {
    if (!inside_while) {
        printf("\nSyntax error in line %d. Using 'continue while' outside the while loop.\n", TokenTable[pos].line);
        exit(1);
    }
    match(Continue);
    match(While);
}

void exit_while() {
    if (!inside_while) {
        printf("\nSyntax error in line %d. Using 'exit while' outside the while loop.\n", TokenTable[pos].line);
        exit(1);
    }
    match(Exit);
    match(While);
}

void break_mark() {
    if (!inside_for_repeat) {
        printf("\nSyntax error in line %d. Using 'break' outside the for or repeat loop.\n", TokenTable[pos].line);
        exit(1);
    }
    match(Break);
}

void goto_mark() {
    match(Identifier);
    match(Colon);
}

void goto_statement() {
    match(Goto);
    match(Identifier);
}

// <присвоЇнн€> ::= <≥дентиф≥катор> ':=' <арифметичний вираз>
void assignment()
{
    arithmetic_expression();
    match(Assign);
    match(Identifier);
}

// <арифметичний вираз> ::= <доданок> { ('+' | '-') <доданок> }
void arithmetic_expression()
{
    term();
    while (TokenTable[pos].type == Add || TokenTable[pos].type == Sub)
    {
        pos++;
        term();
    }
}

// <доданок> ::= <множник> { ('*' | '/') <доданок> }
void term()
{
    factor();
    while (TokenTable[pos].type == Mul || TokenTable[pos].type == Div || TokenTable[pos].type == Mod)
    {
        pos++;
        term();
    }
}

// <множник> ::= <≥дентиф≥катор> | <число> | '(' <арифметичний вираз> ')'
void factor()
{
    if (TokenTable[pos].type == Identifier)
    {
        match(Identifier);
    }
    else
        if (TokenTable[pos].type == Number)
        {
            match(Number);
        }
        else
            if (TokenTable[pos].type == LBraket)
            {
                match(LBraket);
                arithmetic_expression();
                match(RBraket);
            }
            else
            {
                printf("\nSyntax error in line %d with name %s : A multiplier was expected.\n", TokenTable[pos].line, TokenTable[pos].name);
                exit(1);
            }
}

// <вв≥д> ::= 'input' <≥дентиф≥катор>
void input()
{
    match(Input);
    match(Identifier);
    match(Semicolon);
}

// <вив≥д> ::= 'output' <≥дентиф≥катор>
void output()
{
    match(Output);
    arithmetic_expression();
    match(Semicolon);
}

// <умовний оператор> ::= 'if' <лог≥чний вираз> 'then' <оператор> [ 'else' <оператор> ]
void conditional()
{
    match(If);
    logical_expression();
    if_for_body();
    match(Semicolon);
    if (TokenTable[pos].type == Else)
    {
        pos++;
        if_for_body();
        match(Semicolon);
    }
}

// <лог≥чний вираз> ::= <вираз ≤> { '|' <вираз ≤> }
void logical_expression()
{
    and_expression();
    while (TokenTable[pos].type == Or)
    {
        pos++;
        and_expression();
    }
}

// <вираз ≤> ::= <пор≥вн€нн€> { '&' <вираз ≤> }
void and_expression()
{
    comparison();
    while (TokenTable[pos].type == And)
    {
        pos++;
        and_expression();
    }
}

// <пор≥вн€нн€> = <операц≥€ пор≥вн€нн€> | С!С С(С <лог≥чний вираз> С)С | С(С <лог≥чний вираз> С)С
// <операц≥€ пор≥вн€нн€> = <арифметичний вираз> <менше-б≥льше> <арифметичний вираз>
// <менше-б≥льше> = С>С | С<С | С=С | С<>С
void comparison()
{
    if (TokenTable[pos].type == Not)
    {
        // ¬ар≥ант: ! (<лог≥чний вираз>)
        pos++;
        match(LBraket);
        logical_expression();
        match(RBraket);
    }
    else
        if (TokenTable[pos].type == LBraket)
        {
            // ¬ар≥ант: ( <лог≥чний вираз> )
            pos++;
            logical_expression();
            match(RBraket);
        }
        else
        {
            // ¬ар≥ант: <арифметичний вираз> <менше-б≥льше> <арифметичний вираз>
            arithmetic_expression();
            if (TokenTable[pos].type == Greate || TokenTable[pos].type == Less ||
                TokenTable[pos].type == Equality || TokenTable[pos].type == NotEquality)
            {
                pos++;
                arithmetic_expression();
            }
            else
            {
                printf("\nSyntax error: A comparison operation is expected.\n");
                exit(1);
            }
        }
}
/*
// <складений оператор> ::= 'start' <т≥ло програми> 'stop'
void compound_statement()
{
    match(StartProgram);
    program_body();
    match(EndProgram);
}*/

unsigned int LabelIdentification(Label LabelTable[], Token TokenTable[], unsigned int tokenCount)
{
    unsigned int labelCount = 0;
    unsigned int i = 5;
    for (; i < tokenCount; i++) {
        if (TokenTable[i].type == Colon && TokenTable[i - 1].type == Identifier)
            strcpy_s(LabelTable[labelCount++].name, TokenTable[i - 1].name);
    }

    for (i = 5; i < tokenCount; i++) {
        if (TokenTable[i].type == Goto && TokenTable[i + 1].type == Identifier){
            i++;
            bool undeclared = true;
            for (unsigned int j = 0; j < labelCount; j++) {
                if (!strcmp(TokenTable[i].name, LabelTable[j].name)) {
                    undeclared = false;
                    break;
                }
            }
            if(undeclared)
                printf("\nIn line %d, an undeclared label \"%s\"!", TokenTable[i].line, TokenTable[i].name);
        }
    }

    return labelCount;
}

const char* keywords[25] = {
    "or",
    "and",
    "add",
    "sub",
    "else", 
    "if", 
    "print", 
    "scan", 
    "finish", 
    "integer16", 
    "var", 
    "start", 
    "program", 
    "break", 
    "continue", 
    "goto", 
    "end", 
    "while", 
    "until", 
    "repeat", 
    "do", 
    "to", 
    "downto", 
    "for", 
    "exit"
};

// функц≥€ записуЇ оголошен≥ ≥дентиф≥катори в таблицю ≥дентиф≥катор≥в IdTable
// повертаЇ к≥льк≥сть ≥дентиф≥катор≥в
// перев≥р€Ї чи ус≥ використан≥ ≥дентиф≥катори оголошен≥
unsigned int IdIdentification(Id IdTable[], Token TokenTable[], unsigned int tokenCount)
{
    unsigned int idCount = 0;
    unsigned int i = 0;

    while (TokenTable[i++].type != Variable);

    if (TokenTable[i++].type == Type)
    {
        while (TokenTable[i].type != Semicolon)
        {
            if (TokenTable[i].type == Identifier)
            {
                int yes = 0;
                for (unsigned int j = 0; j < idCount; j++)
                {
                    if (!strcmp(TokenTable[i].name, IdTable[j].name))
                    {
                        yes = 1;
                        break;
                    }
                }
                if (yes == 1)
                {
                    printf("\nidentifier \"%s\" is already declared !\n", TokenTable[i].name);
                    return idCount;
                }


                if (idCount < MAX_IDENTIFIER)
                {
                    strcpy_s(IdTable[idCount++].name, TokenTable[i++].name);
                }
                else
                {
                    printf("\nToo many identifiers !\n");
                    return idCount;
                }
            }
            else
                i++;
        }
    }


    for (; i < tokenCount; i++)
    {
        if (TokenTable[i].type == Identifier && TokenTable[i + 1].type != Colon && TokenTable[i - 1].type != Goto)
        {
            int yes = 0;
            for (unsigned int j = 0; j < idCount; j++)
            {
                if (!strcmp(TokenTable[i].name, IdTable[j].name))
                {
                    yes = 1;
                    break;
                }
            }
            if (yes == 0)
                printf("\nIn line %d, an undeclared identifier \"%s\"!", TokenTable[i].line, TokenTable[i].name);
        }

    }
    // перев≥рка чи ≥м'€ ≥дентиф≥катора не зб≥гаЇтьс€ з ≥менем ключового слова
    for (int k = 0; k < tokenCount; k++) {
        for (int l = 0; l < 25; l++) {
            if (TokenTable[k].type == Identifier) {
                if (!strcmp(TokenTable[k].name, keywords[l])) {
                    printf("\nIn line % d, an identifier has a keyword name: \"%s\"!", TokenTable[k].line, TokenTable[k].name);
                }
            }
        }
    }

    return idCount; // ѕовертаЇ к≥льк≥сть  ≥дентиф≥катор≥в
}

void IdLabelCollision(Id IdTable[], Label LabelTable[], unsigned int ids, unsigned int labels) {
    for (int i = 0; i < ids; i++) {
        for (int l = 0; l < labels; l++) {
            if (!strcmp(LabelTable[l].name, IdTable[i].name)) {
                printf("\nLabel \"%s\" collides whith an identifier!", LabelTable[l].name);
            }
        }
    }
}
