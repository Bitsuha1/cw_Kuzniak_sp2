#pragma once

#define MAX_TOKENS 1000
#define MAX_IDENTIFIER 10

// �������������, ��� ����� �� ������ ���� ������
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



