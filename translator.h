#pragma once

#define MAX_TOKENS 1000
#define MAX_IDENTIFIER 10

// перерахування, яке описує всі можливі типи лексем
enum TypeOfTokens
{
	Mod,
	Exit,
	For,
	To,
	Downto,
	Do,
	Repeat,
	Until,
	While,
	End,
	Goto,
	Continue,
	Break,
	ProgramName,
	StartProgram, // start
	Variable, // var
	Type, // integer
	EndProgram, // stop
	Input, // input
	Output, // output
	If, // if
	Colon, // then
	Else, // else
	Identifier, // Identifier
	Number, // number
	Assign, // :=
	Add, // +
	Sub, // -
	Mul, // *
	Div, // /
	Equality, // =
	NotEquality, // <>
	Greate, // >
	Less, // <
	Not, // !
	And, // &
	Or, // |
	LBraket, // (
	RBraket, // )
	Semicolon, // ;
	Comma, // ,
	Unknown
};

// структура для зберігання інформації про лексему
struct Token
{
    char name[16];      // ім'я лексеми
    int value;          // значення лексеми (для цілих констант)
    int line;           // номер рядка
    TypeOfTokens type;  // тип лексеми
};

// структура для зберігання інформації про ідентифікатор
struct Id
{
    char name[16];
};

struct Label
{
	char name[16];
};

// перерахування, яке описує стани лексичного аналізатора
enum States
{
    Start,      // початок виділення чергової лексеми    
    Finish,     // кінець виділення чергової лексеми
    Letter,     // опрацювання слів (ключові слова і ідентифікатори)
    Digit,      // опрацювання цифри
    Separators, // видалення пробілів, символів табуляції і переходу на новий рядок
    Another,    // опрацювання інших символів
    EndOfFile,  // кінець файлу
    SComment,   // початок коментаря
    Comment     // видалення коментаря
};


// перерахування, яке описує всі можливі вузли абстрактного синтаксичного дерева
enum TypeOfNodes
{
    program_node,
    var_node,
    input_node,
    output_node,
    if_node,
    then_node,
    id_node,
    num_node,
    assign_node,
    add_node,
    sub_node,
    mul_node,
    div_node,
    or_node,
    and_node,
    not_node,
    cmp_node,
    statement_node,
    compount_node,
	mod_node,
	to_node,
	downto_node,
	for_node,
	label_node,
	goto_node,
	while_node,
	repeat_node,
	continue_node,
	exit_node,
	break_node
};

// структура, яка описує вузол абстрактного синтаксичного дерева (AST)
struct ASTNode
{
    TypeOfNodes nodetype;    // Тип вузла
    char name[16];           // Ім'я вузла
    struct ASTNode* left;    // Лівий нащадок
    struct ASTNode* right;   // Правий нащадок
};

// функція отримує лексеми з вхідного файлу F і записує їх у таблицю лексем TokenTable
// результат функції - кількість лексем
unsigned int GetTokens(FILE* F, Token TokenTable[]);

// функція друкує таблицю лексем на екран
void PrintTokens(Token TokenTable[], unsigned int TokensNum);

// функція друкує таблицю лексем у файл
void PrintTokensToFile(char* FileName, Token TokenTable[], unsigned int TokensNum);

// синтаксичний аналіз методом рекурсивного спуску
// вхідні дані - глобальна таблиця лексем TokenTable
void Parser();

// функція синтаксичного аналізу і створення абстрактного синтаксичного дерева
ASTNode* ParserAST();

// функція знищення дерева
void destroyTree(ASTNode* root);

// функція для друку AST у вигляді дерева на екран
void PrintAST(ASTNode* node, int level);

// функція для друку AST у вигляді дерева у файл
void PrintASTToFile(ASTNode* node, int level, FILE* outFile);

// Рекурсивна функція для генерації коду з AST
void generateCodefromAST(ASTNode* node, FILE* output);

// функція для генерації коду
void generateCCode(FILE* outFile);



