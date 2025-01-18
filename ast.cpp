#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "translator.h"

// таблиц€ лексем
extern Token* TokenTable;
// к≥льк≥сть лексем
extern unsigned int TokensNum;

static int pos = 0;

// функц≥€ створенн€ вузла AST
ASTNode* createNode(TypeOfNodes type, const char* name, ASTNode* left, ASTNode* right)
{
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->nodetype = type;
    strcpy_s(node->name, name);
    node->left = left;
    node->right = right;
    return node;
}

//CreateWhileNode

// функц≥€ знищенн€ дерева
void destroyTree(ASTNode* root)
{
    if (root == NULL)
        return;

    // –екурсивно знищуЇмо л≥ве ≥ праве п≥ддерево
    destroyTree(root->left);
    destroyTree(root->right);

    // «в≥льн€Їмо пам'€ть дл€ поточного вузла
    free(root);
}

// наб≥р функц≥й дл€ рекурсивного спуску 
// на кожне правило - окрема функц≥€
ASTNode* program();
ASTNode* variable_declaration();
ASTNode* variable_list();
ASTNode* program_body();
ASTNode* statement();
ASTNode* assignment();
ASTNode* arithmetic_expression();
ASTNode* term();
ASTNode* factor();
ASTNode* input();
ASTNode* output();
ASTNode* conditional();
ASTNode* logical_expression();
ASTNode* and_expression();
ASTNode* comparison();
ASTNode* for_loop();
ASTNode* goto_statement();
ASTNode* label();
ASTNode* while_loop();
ASTNode* while_body();
ASTNode* continue_while();
ASTNode* exit_while();
ASTNode* break_mark();
ASTNode* repeat_loop();
ASTNode* if_for_body();

// функц≥€ синтаксичного анал≥зу ≥ створенн€ абстрактного синтаксичного дерева
ASTNode* ParserAST()
{
    ASTNode* tree = program();

    printf("\nParsing completed. AST created.\n");

    return tree;
}

static void match(TypeOfTokens expectedType)
{
    if (TokenTable[pos].type == expectedType)
        pos++;
    else
    {
        printf("\nSyntax error in line %d: Expected another type of lexeme. %d, %s\n", TokenTable[pos].line, expectedType, TokenTable[pos].name);
        exit(10);
    }
}

// <програма> = 'start' 'var' <оголошенн€ зм≥нних> ';' <т≥ло програми> 'stop'
ASTNode* program()
{
    match(ProgramName);
    match(Identifier);
    match(Semicolon);
    match(StartProgram);
    match(Variable);
    ASTNode* declarations = variable_declaration();
    match(Semicolon);
    ASTNode* body = program_body();
    match(EndProgram);
    return createNode(program_node, "program", declarations, body);
}

// <оголошенн€ зм≥нних> = [<тип даних> <список зм≥нних>]
ASTNode* variable_declaration()
{
    if (TokenTable[pos].type == Type)
    {
        pos++;
        return variable_list();
    }
    return NULL;
}

// <список зм≥нних> = <≥дентиф≥катор> { ',' <≥дентиф≥катор> }
ASTNode* variable_list()
{
    match(Identifier);
    ASTNode* id = createNode(id_node, TokenTable[pos - 1].name, NULL, NULL);
    ASTNode* list = list = createNode(var_node, "var", id, NULL);
    while (TokenTable[pos].type == Comma)
    {
        match(Comma);
        match(Identifier);
        id = createNode(id_node, TokenTable[pos - 1].name, NULL, NULL);
        list = createNode(var_node, "var", id, list);
    }
    return list;
}

// <т≥ло програми> = <оператор> ';' { <оператор> ';' }
ASTNode* program_body()
{
    ASTNode* body = statement();
    while (TokenTable[pos].type != EndProgram)
    {
        ASTNode* nextStmt = statement();
        //match(Semicolon);
        body = createNode(statement_node, "statement", body, nextStmt);
    }
    return body;
}

ASTNode* while_body()
{
    ASTNode* body = statement();
    while (TokenTable[pos].type != End)
    {
        ASTNode* nextStmt = statement();
        //match(Semicolon);
        body = createNode(statement_node, "statement", body, nextStmt);
    }
    return body;
}

ASTNode* if_for_body()
{
    ASTNode* body = NULL;
    if (TokenTable[pos].type != Semicolon) {
        body = statement();
    }
    while (TokenTable[pos].type != Semicolon)
    {
        ASTNode* nextStmt = statement();
        //match(Semicolon);
        body = createNode(statement_node, "statement", body, nextStmt);
    }
    return body;
}

ASTNode* repeat_body()
{
    ASTNode* body = statement();
    while (TokenTable[pos].type != Until)
    {
        ASTNode* nextStmt = statement();
        //match(Semicolon);
        body = createNode(statement_node, "statement", body, nextStmt);
    }
    return body;
}

// <оператор> = <присвоЇнн€> | <вв≥д> | <вив≥д> | <умовний оператор> | <складений оператор>
ASTNode* statement()
{
    switch (TokenTable[pos].type)
    {
    case Input: return input();
    case Output: return output();
    case If: return conditional();
    case For: return for_loop();
    case Repeat: return repeat_loop();
    case Goto: return goto_statement();
    case While: return while_loop();
    case Exit: return exit_while();
    case Continue: return continue_while();
    case Break: return break_mark();
    default:
    {
        if (TokenTable[pos + 1].type == Colon)
            return label();
        ASTNode* assign = assignment();
        return assign;
    }
    }
}

ASTNode* for_loop() 
{
    match(For);
    ASTNode* counter = NULL;
    ASTNode* assign = assignment();
    if (TokenTable[pos].type == To)
    {
        pos++;
        counter = createNode(to_node, "to", assign, arithmetic_expression());
    }
    else
    {
        match(Downto);
        counter = createNode(downto_node, "downto", assign, arithmetic_expression());
    }
    match(Do);
    ASTNode* body = if_for_body();
    match(Semicolon);
    return createNode(for_node, "for", counter, body);
}

ASTNode* repeat_loop() {
    match(Repeat);
    ASTNode* body = repeat_body();
    match(Until);
    ASTNode* condition = logical_expression();
    return createNode(repeat_node, "repeat", body, condition);

}

ASTNode* while_loop() {
    match(While);
    ASTNode* condition = logical_expression();
    match(Do);
    ASTNode* body = while_body();
    match(End);
    match(While);
    return createNode(while_node, "while", condition, body);
}

ASTNode* continue_while() {
    match(Continue);
    match(While);
    return createNode(continue_node, "continue", NULL, NULL);
}

ASTNode* exit_while() {
    match(Exit);
    match(While);
    return createNode(exit_node, "exit", NULL, NULL);
}

ASTNode* label() {
    ASTNode* node_label = createNode(label_node, TokenTable[pos].name, NULL, NULL);
    match(Identifier);
    match(Colon);
    return node_label;
}

ASTNode* goto_statement() {
    match(Goto);
    ASTNode* node_label = createNode(label_node, TokenTable[pos].name, NULL, NULL);
    match(Identifier);
    return createNode(goto_node, "goto", node_label, NULL);
}

ASTNode* break_mark() {
    match(Break);
    return createNode(break_node, "break", NULL, NULL);
}

// <присвоЇнн€> = <≥дентиф≥катор> ':=' <арифметичний вираз>
ASTNode* assignment()
{
    ASTNode* expr = arithmetic_expression();
    match(Assign);
    ASTNode* id = createNode(id_node, TokenTable[pos].name, NULL, NULL);
    match(Identifier); 
    return createNode(assign_node, "==>", expr, id);
}

// <арифметичний вираз> = <доданок> { ('+' | '-') <доданок> }
ASTNode* arithmetic_expression()
{
    ASTNode* left = term();
    while (TokenTable[pos].type == Add || TokenTable[pos].type == Sub)
    {
        TypeOfTokens op = TokenTable[pos].type;
        match(op);
        ASTNode* right = term();
        if (op == Add)
            left = createNode(add_node, "add", left, right);
        else
            left = createNode(sub_node, "sub", left, right);
    }
    return left;
}

// <доданок> = <множник> { ('*' | '/') <множник> }
ASTNode* term()
{
    ASTNode* left = factor();
    while (TokenTable[pos].type == Mul || TokenTable[pos].type == Div || TokenTable[pos].type == Mod)
    {
        TypeOfTokens op = TokenTable[pos].type;
        match(op);
        ASTNode* right = factor();
        if (op == Mul)
            left = createNode(mul_node, "*", left, right);
        else if (op == Div)
            left = createNode(div_node, "/", left, right);
        else
            left = createNode(mod_node, "%", left, right);
    }
    return left;
}

// <множник> = <≥дентиф≥катор> | <число> | '(' <арифметичний вираз> ')'
ASTNode* factor()
{
    if (TokenTable[pos].type == Identifier)
    {
        ASTNode* id = createNode(id_node, TokenTable[pos].name, NULL, NULL);
        match(Identifier);
        return id;
    }
    else
        if (TokenTable[pos].type == Number)
        {
            ASTNode* num = createNode(num_node, TokenTable[pos].name, NULL, NULL);
            match(Number);
            return num;
        }
        else
            if (TokenTable[pos].type == LBraket)
            {
                match(LBraket);
                ASTNode* expr = arithmetic_expression();
                match(RBraket);
                return expr;
            }
            else
            {
                printf("\nSyntax error in line %d: A multiplier was expected.\n", TokenTable[pos].line);
                exit(11);
            }
}

// <вв≥д> = 'input' <≥дентиф≥катор>
ASTNode* input()
{
    match(Input);
    ASTNode* id = createNode(id_node, TokenTable[pos].name, NULL, NULL);
    match(Identifier);
    match(Semicolon);
    return createNode(input_node, "input", id, NULL);
}

// <вив≥д> = 'output' <≥дентиф≥катор>
ASTNode* output()
{
    match(Output);
    ASTNode* id = arithmetic_expression();// createNode(id_node, TokenTable[pos].name, NULL, NULL);
    //match(Identifier);
    match(Semicolon);
    return createNode(output_node, "output", id, NULL);
}

// <умовний оператор> = 'if' <лог≥чний вираз> 'then' <оператор> [ 'else' <оператор> ]
ASTNode* conditional()
{
    match(If);
    ASTNode* condition = logical_expression();
    ASTNode* ifBranch = if_for_body();
    match(Semicolon);
    ASTNode* elseBranch = NULL;
    if (TokenTable[pos].type == Else)
    {
        match(Else);
        elseBranch = if_for_body();
        match(Semicolon);
    }
    return createNode(if_node, "if", condition, createNode(then_node, "statement", ifBranch, elseBranch));
}

// <лог≥чний вираз> = <вираз ≤> { '|' <вираз ≤> }
ASTNode* logical_expression()
{
    ASTNode* left = and_expression();
    while (TokenTable[pos].type == Or)
    {
        match(Or);
        ASTNode* right = and_expression();
        left = createNode(or_node, "|", left, right);
    }
    return left;
}

// <вираз ≤> = <пор≥вн€нн€> { '&' <пор≥вн€нн€> }
ASTNode* and_expression()
{
    ASTNode* left = comparison();
    while (TokenTable[pos].type == And)
    {
        match(And);
        ASTNode* right = comparison();
        left = createNode(and_node, "&", left, right);
    }
    return left;
}

// <пор≥вн€нн€> = <операц≥€ пор≥вн€нн€> | С!С С(С <лог≥чний вираз> С)С | С(С <лог≥чний вираз> С)С
// <операц≥€ пор≥вн€нн€> = <арифметичний вираз> <менше-б≥льше> <арифметичний вираз>
// <менше-б≥льше> = С>С | С<С | С=С | С<>С
ASTNode* comparison()
{
    if (TokenTable[pos].type == Not)
    {
        // ¬ар≥ант: ! (<лог≥чний вираз>)
        match(Not);
        match(LBraket);
        ASTNode* expr = logical_expression();
        match(RBraket);
        return createNode(not_node, "!", expr, NULL);
    }
    else
        if (TokenTable[pos].type == LBraket)
        {
            // ¬ар≥ант: ( <лог≥чний вираз> )
            match(LBraket);
            ASTNode* expr = logical_expression();
            match(RBraket);
            return expr;  // ѕовертаЇмо вираз у дужках €к п≥ддерево
        }
        else
        {
            // ¬ар≥ант: <арифметичний вираз> <менше-б≥льше> <арифметичний вираз>
            ASTNode* left = arithmetic_expression();
            if (TokenTable[pos].type == Greate || TokenTable[pos].type == Less ||
                TokenTable[pos].type == Equality || TokenTable[pos].type == NotEquality)
            {
                TypeOfTokens op = TokenTable[pos].type;
                char operatorName[16];
                strcpy_s(operatorName, TokenTable[pos].name);
                match(op);
                ASTNode* right = arithmetic_expression();
                return createNode(cmp_node, operatorName, left, right);
            }
            else
            {
                printf("\nSyntax error: A comparison operation is expected.\n");
                exit(12);
            }
        }
}


// функц≥€ дл€ друку AST у вигл€д≥ дерева на екран
void PrintAST(ASTNode* node, int level)
{
    if (node == NULL)
        return;

    // ¬≥дступи дл€ позначенн€ р≥вн€ вузла
    for (int i = 0; i < level; i++)
        printf("|    ");

    // ¬иводимо ≥нформац≥ю про вузол
    printf("|-- %s", node->name);
    printf("\n");

    // –екурсивний друк л≥вого та правого п≥ддерева
    if (node->left || node->right)
    {
        PrintAST(node->left, level + 1);
        PrintAST(node->right, level + 1);
    }
}

// функц≥€ дл€ друку AST у вигл€д≥ дерева у файл
void PrintASTToFile(ASTNode* node, int level, FILE* outFile)
{
    if (node == NULL)
        return;

    // ¬≥дступи дл€ позначенн€ р≥вн€ вузла
    for (int i = 0; i < level; i++)
        fprintf(outFile, "|    ");

    // ¬иводимо ≥нформац≥ю про вузол
    fprintf(outFile, "|-- %s", node->name);
    fprintf(outFile, "\n");

    // –екурсивний друк л≥вого та правого п≥ддерева
    if (node->left || node->right)
    {
        PrintASTToFile(node->left, level + 1, outFile);
        PrintASTToFile(node->right, level + 1, outFile);
    }
}