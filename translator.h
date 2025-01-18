#pragma once

#define MAX_TOKENS 1000
#define MAX_IDENTIFIER 10

// �������������, ��� ����� �� ������ ���� ������
enum TypeOfTokens
{
	Mod, //%
	Exit, // exit (while)
	For, // for
	To, // to
	Downto, // downto
	Do, //do
	Repeat, // repeat
	Until, // until
	While, // while
	End, // end (while)
	Goto, // goto
	Continue, // continue (while)
	Break, // break
	ProgramName, // program
	StartProgram, // start
	Variable, // var
	Type, // integer16
	EndProgram, // finish
	Input, // scan
	Output, // print
	If, // if
	Colon, // :
	Else, // else
	Identifier, // Identifier
	Number, // number
	Assign, // ==>
	Add, // add
	Sub, // sub
	Mul, // *
	Div, // /
	Equality, // =
	NotEquality, // <>
	Greate, // >
	Less, // <
	Not, // not
	And, // and
	Or, // or
	LBraket, // (
	RBraket, // )
	Semicolon, // ;
	Comma, // ,
	Unknown
};

// ��������� ��� ��������� ���������� ��� �������
struct Token
{
    char name[16];      // ��'� �������
    int value;          // �������� ������� (��� ����� ��������)
    int line;           // ����� �����
    TypeOfTokens type;  // ��� �������
};

// ��������� ��� ��������� ���������� ��� �������������
struct Id
{
    char name[16];
};

struct Label
{
	char name[16];
};

// �������������, ��� ����� ����� ���������� ����������
enum States
{
    Start,      // ������� �������� ������� �������    
    Finish,     // ����� �������� ������� �������
    Letter,     // ����������� ��� (������ ����� � ��������������)
    Digit,      // ����������� �����
    Separators, // ��������� ������, ������� ��������� � �������� �� ����� �����
    Another,    // ����������� ����� �������
    EndOfFile,  // ����� �����
    SComment,   // ������� ���������
    Comment     // ��������� ���������
};


// �������������, ��� ����� �� ������ ����� ������������ ������������� ������
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

// ���������, ��� ����� ����� ������������ ������������� ������ (AST)
struct ASTNode
{
    TypeOfNodes nodetype;    // ��� �����
    char name[16];           // ��'� �����
    struct ASTNode* left;    // ˳��� �������
    struct ASTNode* right;   // ������ �������
};

// ������� ������ ������� � �������� ����� F � ������ �� � ������� ������ TokenTable
// ��������� ������� - ������� ������
unsigned int GetTokens(FILE* F, Token TokenTable[]);

// ������� ����� ������� ������ �� �����
void PrintTokens(Token TokenTable[], unsigned int TokensNum);

// ������� ����� ������� ������ � ����
void PrintTokensToFile(char* FileName, Token TokenTable[], unsigned int TokensNum);

// ������������ ����� ������� ������������ ������
// ����� ��� - ��������� ������� ������ TokenTable
void Parser();

// ������� ������������� ������ � ��������� ������������ ������������� ������
ASTNode* ParserAST();

// ������� �������� ������
void destroyTree(ASTNode* root);

// ������� ��� ����� AST � ������ ������ �� �����
void PrintAST(ASTNode* node, int level);

// ������� ��� ����� AST � ������ ������ � ����
void PrintASTToFile(ASTNode* node, int level, FILE* outFile);

// ���������� ������� ��� ��������� ���� � AST
void generateCodefromAST(ASTNode* node, FILE* output);

// ������� ��� ��������� ����
void generateCCode(FILE* outFile);



