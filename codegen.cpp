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
// к≥льк≥сть ≥дентиф≥катор≥в
extern unsigned int IdNum;

static int pos = 5;

// наб≥р функц≥й дл€ рекурсивного спуску 
// на кожне правило - окрема функц≥€

void variable_declaration(FILE* outFile);
void variable_list(FILE* outFile);
void program_body(FILE* outFile);
void statement(FILE* outFile);
void assignment(FILE* outFile);
void arithmetic_expression(FILE* outFile);
void term(FILE* outFile);
void factor(FILE* outFile);
void input(FILE* outFile);
void output(FILE* outFile);
void conditional(FILE* outFile);
void logical_expression(FILE* outFile);
void and_expression(FILE* outFile);
void comparison(FILE* outFile);
void compound_statement(FILE* outFile);
void for_loop(FILE* outFile);
void while_loop(FILE* outFile);
void repeat_loop(FILE* outFile);
void goto_statement(FILE* outFile);
void label(FILE* outFile);
void exit_while(FILE* outFile);
void continue_while(FILE* outFile);
void break_mark(FILE* outFile);

void generateCCode(FILE* outFile)
{
    fprintf(outFile, "#include <stdio.h>\n\n");
    fprintf(outFile, "int main() \n{\n");
    variable_declaration(outFile);
    fprintf(outFile, ";\n");
    pos++;
    program_body(outFile);
    fprintf(outFile, "   return 0;\n");
    fprintf(outFile, "}\n");
}


// <оголошенн€ зм≥нних> = [<тип даних> <список зм≥нних>]
void variable_declaration(FILE* outFile)
{
    if (TokenTable[pos].type == Type)
    {
        fprintf(outFile, "   int ");
        pos++;
        variable_list(outFile);
    }
}

// <список зм≥нних> = <≥дентиф≥катор> { ',' <≥дентиф≥катор> }
void variable_list(FILE* outFile)
{
    fprintf(outFile, TokenTable[pos++].name);
    while (TokenTable[pos].type == Comma)
    {
        fprintf(outFile, ", ");
        pos++;
        fprintf(outFile, TokenTable[pos++].name);
    }
}

// <т≥ло програми> = <оператор> ';' { <оператор> ';' }
void program_body(FILE* outFile)
{
    do
    {
        statement(outFile);
        //pos++;
    } while (TokenTable[pos].type != EndProgram);
}

void while_body(FILE* outFile)
{
    do
    {
        statement(outFile);
        //pos++;
    } while (TokenTable[pos].type != End);
}

void repeat_body(FILE* outFile)
{
    do
    {
        statement(outFile);
        //pos++;
    } while (TokenTable[pos].type != Until);
}

// <оператор> = <присвоЇнн€> | <вв≥д> | <вив≥д> | <умовний оператор> | <складений оператор>
void statement(FILE* outFile)
{
    switch (TokenTable[pos].type)
    {
    case Input: input(outFile); break;
    case Output: output(outFile); break;
    case If: conditional(outFile); break;
    case For: for_loop(outFile); break;
    case Repeat: repeat_loop(outFile); break;
    case While: while_loop(outFile); break;
    case Goto: goto_statement(outFile); break;
    case Exit: exit_while(outFile); break;
    case Break: break_mark(outFile); break;
    case Continue: continue_while(outFile); break;
    case StartProgram: compound_statement(outFile); break;
    default: {
        if (TokenTable[pos + 1].type == Colon) {
            label(outFile);
            break;
        }
        assignment(outFile);
        break;
        }
    }
}

// <присвоЇнн€> = <≥дентиф≥катор> ':=' <арифметичний вираз>
void assignment(FILE* outFile)
{
    fprintf(outFile, "   ");
    int i = pos;
    for (; TokenTable[i].type != Assign; i++) {}
    fprintf(outFile, TokenTable[i + 1].name);
    fprintf(outFile, " = ");
    arithmetic_expression(outFile);
    pos = pos + 3;
    fprintf(outFile, ";\n");
}

// <арифметичний вираз> = <доданок> { ('+' | '-') <доданок> }
void arithmetic_expression(FILE* outFile)
{
    term(outFile);
    while (TokenTable[pos].type == Add || TokenTable[pos].type == Sub)
    {
        if (TokenTable[pos].type == Add)
            fprintf(outFile, " + ");
        else
            fprintf(outFile, " - ");
        pos++;
        term(outFile);
    }
}

// <доданок> = <множник> { ('*' | '/') <множник> }
void term(FILE* outFile)
{
    factor(outFile);
    while (TokenTable[pos].type == Mul || TokenTable[pos].type == Div || TokenTable[pos].type == Mod)
    {
        if (TokenTable[pos].type == Mul)
            fprintf(outFile, " * ");
        else if (TokenTable[pos].type == Div)
            fprintf(outFile, " / ");
        else
            fprintf(outFile, " %% ");
        pos++;
        factor(outFile);
    }
}

// <множник> = <≥дентиф≥катор> | <число> | '(' <арифметичний вираз> ')'
void factor(FILE* outFile)
{
    if (TokenTable[pos].type == Identifier || TokenTable[pos].type == Number)
        fprintf(outFile, TokenTable[pos++].name);
    else
        if (TokenTable[pos].type == LBraket)
        {
            fprintf(outFile, "(");
            pos++;
            arithmetic_expression(outFile);
            fprintf(outFile, ")");
            pos++;
        }
}

// <вв≥д> = 'input' <≥дентиф≥катор>
void input(FILE* outFile)
{
    fprintf(outFile, "   printf(\"Enter ");
    fprintf(outFile, TokenTable[pos + 1].name);
    fprintf(outFile, ":\");\n");
    fprintf(outFile, "   scanf_s(\"%%d\", &");
    pos++;
    fprintf(outFile, TokenTable[pos++].name);
    fprintf(outFile, ");\n");
    pos++;
}

// <вив≥д> = 'output' <≥дентиф≥катор>
void output(FILE* outFile)
{
    fprintf(outFile, "   printf(\"%%d\\n\", ");
    pos++;
    arithmetic_expression(outFile);
    fprintf(outFile, ");\n");
    pos++;
}

void label(FILE* outFile) {
    fprintf(outFile, TokenTable[pos++].name);
    fprintf(outFile, ":\n");
    pos++;
}

void goto_statement(FILE* outFile) {
    fprintf(outFile, "   goto ");
    pos++;
    fprintf(outFile, TokenTable[pos++].name);
    fprintf(outFile, ";\n");
    pos++;
}

// <умовний оператор> = 'if' <лог≥чний вираз> 'then' <оператор> [ 'else' <оператор> ]
void conditional(FILE* outFile)
{
    fprintf(outFile, "   if (");
    pos++;
    logical_expression(outFile);
    fprintf(outFile, ")\n");
    statement(outFile);
    if (TokenTable[pos].type == Else)
    {
        fprintf(outFile, "   else\n");
        pos++;
        statement(outFile);
    }
}

void for_loop(FILE* outFile) {
    fprintf(outFile, "   for (");
    pos++;
    int i = pos;
    for (; TokenTable[i].type != Assign; i++){}
    char* ident = TokenTable[i + 1].name;
    fprintf(outFile, ident);
    fprintf(outFile, " = ");
    arithmetic_expression(outFile);
    pos = pos + 2;
    fprintf(outFile, ";");
    fprintf(outFile, ident);
    if (TokenTable[pos].type == To) {
        pos++;
        fprintf(outFile, " < ");
        arithmetic_expression(outFile);
        fprintf(outFile, ";");
        fprintf(outFile, ident);
        fprintf(outFile, "++");
    }
    else {
        pos++;
        fprintf(outFile, " >= ");
        arithmetic_expression(outFile);
        fprintf(outFile, ";");
        fprintf(outFile, ident);
        fprintf(outFile, "--");
    }
    fprintf(outFile, ")\n");
    pos++;
    statement(outFile);
}

void while_loop(FILE* outFile) {
    fprintf(outFile, "   while (");
    pos++;
    logical_expression(outFile);
    fprintf(outFile, ")\n   {\n");
    pos++;
    while_body(outFile);
    fprintf(outFile, "   }\n");
    pos = pos + 2;
}

void repeat_loop(FILE* outFile) {
    fprintf(outFile, "   do {");
    pos++;
    repeat_body(outFile);
    fprintf(outFile, "   } while (");
    pos++;
    logical_expression(outFile);
    fprintf(outFile, ");\n");
    pos++;
}

void exit_while(FILE* outFile){
    pos = pos + 3;
    fprintf(outFile, "   break;\n");
}
void continue_while(FILE* outFile){
    pos = pos + 3;
    fprintf(outFile, "   continue;\n");
}
void break_mark(FILE* outFile){
    pos = pos + 2;
    fprintf(outFile, "   break;\n");
}

// <лог≥чний вираз> = <вираз ≤> { '|' <вираз ≤> }
void logical_expression(FILE* outFile)
{
    and_expression(outFile);
    while (TokenTable[pos].type == Or)
    {
        fprintf(outFile, " || ");
        pos++;
        and_expression(outFile);
    }
}

// <вираз ≤> = <пор≥вн€нн€> { '&' <пор≥вн€нн€> }
void and_expression(FILE* outFile)
{
    comparison(outFile);
    while (TokenTable[pos].type == And)
    {
        fprintf(outFile, " && ");
        pos++;
        comparison(outFile);
    }
}

// <пор≥вн€нн€> = <операц≥€ пор≥вн€нн€> | С!С С(С <лог≥чний вираз> С)С | С(С <лог≥чний вираз> С)С
// <операц≥€ пор≥вн€нн€> = <арифметичний вираз> <менше-б≥льше> <арифметичний вираз>
// <менше-б≥льше> = С>С | С<С | С=С | С<>С
void comparison(FILE* outFile)
{
    if (TokenTable[pos].type == Not)
    {
        // ¬ар≥ант: ! (<лог≥чний вираз>)
        fprintf(outFile, "!(");
        pos++;
        pos++;
        logical_expression(outFile);
        fprintf(outFile, ")");
        pos++;
    }
    else
        if (TokenTable[pos].type == LBraket)
        {
            // ¬ар≥ант: ( <лог≥чний вираз> )
            fprintf(outFile, "(");
            pos++;
            logical_expression(outFile);
            fprintf(outFile, ")");
            pos++;
        }
        else
        {
            // ¬ар≥ант: <арифметичний вираз> <менше-б≥льше> <арифметичний вираз>
            arithmetic_expression(outFile);
            if (TokenTable[pos].type == Greate || TokenTable[pos].type == Less ||
                TokenTable[pos].type == Equality || TokenTable[pos].type == NotEquality)
            {
                switch (TokenTable[pos].type)
                {
                case Greate: fprintf(outFile, " > "); break;
                case Less: fprintf(outFile, " < "); break;
                case Equality: fprintf(outFile, " == "); break;
                case NotEquality: fprintf(outFile, " != "); break;
                }
                pos++;
                arithmetic_expression(outFile);
            }
        }
}

// <складений оператор> = 'start' <т≥ло програми> 'stop'
void compound_statement(FILE* outFile)
{
    fprintf(outFile, "   {\n");
    pos++;
    program_body(outFile);
    fprintf(outFile, "   }\n");
    pos++;
}



