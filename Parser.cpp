// This file was generated with Coco/R C++, version: 20101106
// http://www.ssw.uni-linz.ac.at/coco/
// with these defines:


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <sstream>

#include "Parser.h"



//! @cond fileScope
//
//  Create by copying str - only used locally
inline static wchar_t* coco_string_create(const wchar_t* str)
{
	const int len = wcslen(str);
	wchar_t* dst = new wchar_t[len + 1];
	wcsncpy(dst, str, len);
	dst[len] = 0;
	return dst;
}


// Free storage and nullify the argument
inline static void coco_string_delete(wchar_t* &str)
{
	delete[] str;
	str = NULL;
}
//
//! @endcond


// ----------------------------------------------------------------------------
// Parser Implementation
// ----------------------------------------------------------------------------

void Parser::SynErr(int n)
{
	if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
	errDist = 0;
}


void Parser::SemErr(const std::wstring& msg)
{
	if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
	errDist = 0;
}


bool Parser::isUTF8() const
{
	return scanner && scanner->buffer && scanner->buffer->isUTF8();
}


void Parser::Get()
{
	for (;;)
	{
		t = la;
		la = scanner->Scan();
		if (la->kind <= maxT)
		{
			++errDist;
			break;
		}
		if (dummyToken != t)
		{
			dummyToken->kind = t->kind;
			dummyToken->pos = t->pos;
			dummyToken->col = t->col;
			dummyToken->line = t->line;
			dummyToken->next = NULL;
			coco_string_delete(dummyToken->val);
			dummyToken->val = coco_string_create(t->val);
			t = dummyToken;
		}
		la = t;
	}
}


void Parser::Expect(int n)
{
	if (la->kind == n)
	{
		Get();
	}
	else
	{
		SynErr(n);
	}
}


void Parser::ExpectWeak(int n, int follow)
{
	if (la->kind == n)
	{
		Get();
	}
	else
	{
		SynErr(n);
		while (!StartOf(follow))
		{
			Get();
		}
	}
}


bool Parser::WeakSeparator(int n, int syFol, int repFol)
{
	if (la->kind == n)
	{
		Get();
		return true;
	}
	else if (StartOf(repFol))
	{
		return false;
	}
	else
	{
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0)))
		{
			Get();
		}
		return StartOf(syFol);
	}
}


void Parser::B()
{
	ParseList=L"(B ";scopepos=0;
	while (la->kind == 1) {
		Definition();
	}
	Expect(0);
	ParseList+=L")";
	std::wstringstream data;
	data << L"segment .data;" << std::endl << std::endl;
	std::wstringstream bss;
	bss << L"segment .bss;" << std::endl << std::endl;
	std::wstringstream text;
	text << L"segment .text;" << std::endl << std::endl;
	Compile(0, data, bss, text);
	std::wcout << L"%include \"asm_io.inc\"" << std::endl;
	std::wcout << data.str() << std::endl;
	std::wcout << bss.str() << std::endl;
	std::wcout << text.str() << std::endl;
	#ifdef PRINT_PARSE_LIST
	std::wcout << L";" << ParseList << std::endl;
	#endif
}


void Parser::Definition()
{
	Name name; bool isarray; int position;
	isarray=false;
	while (!(la->kind == 0 || la->kind == 1)) {SynErr(67); Get();}
	position=ParseList.length();
	Ident(name);
	if (IsDeclaredGlobal(name))  AlreadyErr(name) ; ParseList.append(name);ParseList.append(L" ");
	if (la->kind == 12) {
		FunctionDefinition();
		ParseList.insert(position,L"(FUNCDEF " ); ParseList.append(L") " );
	} else if (StartOf(1)) {
		ArraySize(isarray);
		ParseList.insert(position,isarray?L"(GARRDEF ":L"(GVARDEF ") ; ParseList.append(L") " );
	} else SynErr(68);
}


void Parser::Ident(Name name)
{
	Expect(1);
	wcscpy(name, t->val) ;
}


void Parser::GotoLabel(Name name)
{
	Expect(2);
	wcscpy(name, t->val) ;
	Expect(7);
}


void Parser::ConstVal()
{
	Name name;
	if (la->kind == 3) {

		Get();
		wcscpy(name, t->val) ; ParseList.append(L"(INT ");
		ParseList.append(name);ParseList.append(L")") ;
	} else if (la->kind == 5) {
		Get();
		wcscpy(name, t->val) ;ParseList.append(L"(STRING `");
		ParseList.append(name);ParseList.append(L"`)") ;
	} else if (la->kind == 6) {
		Get();
		wcscpy(name, t->val) ;ParseList.append(L"(CHAR `");
		ParseList.append(name);ParseList.append(L"`)") ;
	} else if (la->kind == 4) {
		Get();
		wcscpy(name, t->val) ;ParseList.append(L"(ONUMBER ");
		ParseList.append(name);ParseList.append(L")") ;
	} else SynErr(69);
}


void Parser::FunctionDefinition()
{
	scopepos=ParseList.length();
	FunctionHeader();
	FunctionBody();
}


void Parser::ArraySize(bool & isarray)
{
	int position;
	isarray=false;
	if (la->kind == 8) {
		Get();
		if (la->kind == 3) {
			Get();
			isarray=true; ParseList.append(L"(ASIZE ");
			ParseList.append(t->val);ParseList.append(L")") ;
		}
		Expect(9);
	}
	if (StartOf(2)) {
		position=ParseList.length();
		Initializator();
		ParseList.insert(position,L"(INIT " ); ParseList.append(L") " );
		while (la->kind == 10) {
			Get();
			position=ParseList.length();isarray=true;
			Initializator();
			ParseList.insert(position,L"(INIT " ); ParseList.append(L") " );
		}
	}
	Expect(11);
}


void Parser::Initializator()
{
	Name name; 
	if (StartOf(3)) {
		ConstVal();
	} else if (la->kind == 1) {
		Ident(name);
		ParseList.append(L"(SAMEAS ");
		ParseList.append(name);ParseList.append(L")") ;
	} else SynErr(70);
}


void Parser::FunctionHeader()
{
	Expect(12);
	ParseList.append(L"(FHEADER ");
	if (la->kind == 1) {
		FormalParamList();
	}
	ParseList.append(L") " );
	Expect(13);
}


void Parser::FunctionBody()
{

	Statement();
}


void Parser::FormalParamList()
{
	FormalParameter();
	while (la->kind == 10) {
		Get();
		FormalParameter();
	}
}


void Parser::Statement()
{
	bool hasdeclarations; int position;
	while (!(StartOf(4))) {SynErr(71); Get();}
	position=ParseList.length();hasdeclarations=false;
	while (StartOf(5)) {
		if (la->kind == 2 || la->kind == 14 || la->kind == 15) {
			Label();
			hasdeclarations=true;
		} else if (la->kind == 26) {
			ExtrnDeclaration();
			hasdeclarations=true;
		} else {
			AutoDeclaration();
			hasdeclarations=true;
		}
	}
	switch (la->kind) {
	case 1: case 3: case 4: case 5: case 6: case 12: case 48: case 57: case 58: case 59: case 62: case 63: case 64: case 65: {
		StatementExpression();
		break;
	}
	case 16: {
		BreakStatement();
		break;
	}
	case 17: {
		CompoundStatement();
		break;
	}
	case 19: {
		ContinueStatement();
		break;
	}
	case 20: {
		GotoStatement();
		break;
	}
	case 21: {
		IfStatement();
		break;
	}
	case 11: {
		NullStatement();
		break;
	}
	case 23: {
		ReturnStatement();
		break;
	}
	case 24: {
		SwitchStatement();
		break;
	}
	case 25: {
		WhileStatement();
		break;
	}
	default: SynErr(72); break;
	}
	if (hasdeclarations) {
	ParseList.insert(position,L"(DECLSTAT " ); ParseList.append(L") " );}
}


void Parser::FormalParameter()
{
	Name name; 
	Ident(name);
	ParseList.append(L"(FPARAM ");
	ParseList.append(name);ParseList.append(L" ");;ParseList.append(L")") ;
}


void Parser::Label()
{
	Name name;int position;
	if (la->kind == 14) {
		position=ParseList.length();
		Get();
		ConstVal();
		Expect(7);
		ParseList.insert(position,L"(CASE " ); ParseList.append(L" ) " );
	} else if (la->kind == 15) {
		Get();
		Expect(7);
		ParseList.append(L"(DEFAULT) ");
	} else if (la->kind == 2) {
		GotoLabel(name);
		ParseList.append(L"(LABEL ");ParseList.append(name); ;ParseList.append(L" )");
	} else SynErr(73);
}


void Parser::ExtrnDeclaration()
{
	Name name; 
	Expect(26);
	Ident(name);
	ParseList.append(L"(EXTRN ");ParseList.append(name); ;ParseList.append(L" )");
	while (la->kind == 10) {
		Get();
		Ident(name);
		ParseList.append(L"(EXTRN ");ParseList.append(name); ;ParseList.append(L" )");
	}
	Expect(11);
}


void Parser::AutoDeclaration()
{
	Name name; int position; bool isarray;
	Expect(27);
	isarray=false;position=ParseList.length();
	Ident(name);
	ParseList.append(name);ParseList.append(L" ");
	if (la->kind == 3 || la->kind == 8) {
		if (la->kind == 8) {
			Get();
			Expect(3);
			isarray=true; ParseList.append(L"(ASIZE ");
			ParseList.append(t->val);ParseList.append(L")") ;
			Expect(9);
		} else {
			Get();
			ParseList.append(L"(INIT ");
			ParseList.append(t->val);ParseList.append(L")") ;
		}
	}
	ParseList.insert(position,isarray?L"(LARRDEF ":L"(LVARDEF ") ; ParseList.append(L") " );
	while (la->kind == 10) {
		Get();
		isarray=false;position=ParseList.length();
		Ident(name);
		ParseList.append(name);ParseList.append(L" "); 
		if (la->kind == 3 || la->kind == 8) {
			if (la->kind == 8) {
				Get();
				Expect(3);
				isarray=true; ParseList.append(L"(ASIZE");
				ParseList.append(t->val);ParseList.append(L")") ;
				Expect(9);
			} else {
				Get();
				ParseList.append(L"(INIT");
				ParseList.append(t->val);ParseList.append(L")") ;
			}
		}
		ParseList.insert(position,isarray?L"(LARRDEF ":L"(LVARDEF ") ; ParseList.append(L") " );
	}
	Expect(11);
}


void Parser::StatementExpression()
{
	Expression();
	ExpectWeak(11, 6);
}


void Parser::BreakStatement()
{
	Expect(16);
	ParseList.append(L"(BREAK) ");
	ExpectWeak(11, 6);
}


void Parser::CompoundStatement()
{
	Expect(17);
	ParseList.append(L"(BLOCK ");
	while (StartOf(7)) {
		Statement();
	}
	Expect(18);
	ParseList.append(L") ");
}


void Parser::ContinueStatement()
{
	Expect(19);
	ParseList.append(L"(CONTINUE) ");
	ExpectWeak(11, 6);
}


void Parser::GotoStatement()
{
	Name name; 
	Expect(20);
	Ident(name);
	ParseList.append(L"(GOTO ");ParseList.append(name); ;ParseList.append(L")");
	Expect(11);
}


void Parser::IfStatement()
{
	int position; bool haselse;
	haselse=false;
	Expect(21);
	position=ParseList.length();
	Expect(12);
	Expression();
	Expect(13);
	Statement();
	if (la->kind == 22) {
		Get();
		Statement();
		haselse=true;
	}
	ParseList.insert(position,haselse? L"(IFELSE ": L"(IF ");ParseList.append(L")" ); 
}


void Parser::NullStatement()
{
	Expect(11);
}


void Parser::ReturnStatement()
{
	int position; bool paramExists;
	paramExists = false;
	Expect(23);
	position=ParseList.length();
	if (la->kind == 12) {
		ExpectWeak(12, 4);
		Expression();
		paramExists = true;
		ExpectWeak(13, 4);
	}
	ParseList.insert(position, paramExists? L"(RETURNPARAM " : L"(RETURN "); ParseList.append(L")");
	Expect(11);
}


void Parser::SwitchStatement()
{
	int position;
	position=ParseList.length();
	Expect(24);
	Expect(12);
	Expression();
	Expect(13);
	CompoundStatement();
	ParseList.insert(position, L"(SWITCH "); ParseList.append(L")");
}


void Parser::WhileStatement()
{
	int position;
	position=ParseList.length();
	Expect(25);
	Expect(12);
	Expression();
	Expect(13);
	Statement();
	ParseList.insert(position, L"(WHILE "); ParseList.append(L")");
}


void Parser::Expression()
{
	AssignExpr();
}


void Parser::AssignExpr()
{
	int position;
	position=ParseList.length();
	CondExpr();
	if (StartOf(8)) {
		switch (la->kind) {
		case 28: {
			Get();
			AssignExpr();
			if (!Assignable(position)) SemErr(L"Not assignable");
			ParseList.insert(position,L"(MOV " );
			ParseList.append(L") " );
			break;
		}
		case 29: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(MULTMOV " );
			ParseList.append(L") " );
			break;
		}
		case 30: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(DIVMOV " );
			ParseList.append(L") " );
			break;
		}
		case 31: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(MODMOV " );
			ParseList.append(L") " );
			break;
		}
		case 32: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(ADDMOV " );
			ParseList.append(L") " );
			break;
		}
		case 33: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(SUBMOV " );
			ParseList.append(L") " );
			break;
		}
		case 34: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(ANDMOV " );
			ParseList.append(L") " );
			break;
		}
		case 35: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(XORMOV " );
			ParseList.append(L") " );
			break;
		}
		case 36: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(ORMOV " );
			ParseList.append(L") " );
			break;
		}
		case 37: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(LSHIFTMOV " );
			ParseList.append(L") " );
			break;
		}
		case 38: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(RSHIFTMOV " );
			ParseList.append(L") " );
			break;
		}
		case 39: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(ISLESSMOV " );
			ParseList.append(L") " );
			break;
		}
		case 40: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(ISLESSEQUMOV " );
			ParseList.append(L") " );
			break;
		}
		case 41: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(ISGREATERMOV " );
			ParseList.append(L") " );
			break;
		}
		case 42: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(ISGREATEREQUMOV " );
			ParseList.append(L") " );
			break;
		}
		case 43: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(ISEQUMOV " );
			ParseList.append(L") " );
			break;
		}
		case 44: {
			Get();
			AssignExpr();
			ParseList.insert(position,L"(ISNEQUMOV " );
			ParseList.append(L") " );
			break;
		}
		}
	}
}


void Parser::CondExpr()
{
	int position;
	position=ParseList.length();
	OrExpr();
	if (la->kind == 45) {
		Get();
		Expression();
		Expect(7);
		CondExpr();
		ParseList.insert(position,L"(CONDEXPR " );
		ParseList.append(L") " );
	}
}


void Parser::OrExpr()
{
	int position;
	position=ParseList.length();
	XorExpr();
	while (la->kind == 46) {
		Get();
		XorExpr();
		ParseList.insert(position,L"(OR " );
		ParseList.append(L") " );
	}
}


void Parser::XorExpr()
{
	int position;
	position=ParseList.length();
	AndExpr();
	while (la->kind == 47) {
		Get();
		AndExpr();
		ParseList.insert(position,L"(XOR " );
		ParseList.append(L") " );
	}
}


void Parser::AndExpr()
{
	int position;
	position=ParseList.length();
	EqlExpr();
	while (la->kind == 48) {
		Get();
		EqlExpr();
		ParseList.insert(position,L"(AND " );
		ParseList.append(L") " );
	}
}


void Parser::EqlExpr()
{
	int position;
	position=ParseList.length();
	RelExpr();
	while (la->kind == 49 || la->kind == 50) {
		if (la->kind == 49) {
			Get();
			RelExpr();
			ParseList.insert(position,L"(EQU " );
			ParseList.append(L") " );
		} else {
			Get();
			RelExpr();
			ParseList.insert(position,L"(NEQU " );
			ParseList.append(L") " );
		}
	}
}


void Parser::RelExpr()
{
	int position;
	position=ParseList.length();
	ShiftExpr();
	while (StartOf(9)) {
		if (la->kind == 51) {
			Get();
			ShiftExpr();
			ParseList.insert(position,L"(LESSTHAN " );
			ParseList.append(L") " );
		} else if (la->kind == 52) {
			Get();
			ShiftExpr();
			ParseList.insert(position,L"(GREATERTHAN " );
			ParseList.append(L") " );
		} else if (la->kind == 53) {
			Get();
			ShiftExpr();
			ParseList.insert(position,L"(LESSEQUTHAN " );
			ParseList.append(L") " );
		} else {
			Get();
			ShiftExpr();
			ParseList.insert(position,L"(GREATEREQUTHAN " );
			ParseList.append(L") " );
		}
	}
}


void Parser::ShiftExpr()
{
	int position;
	position=ParseList.length();
	AddExpr();
	while (la->kind == 55 || la->kind == 56) {
		if (la->kind == 55) {
			Get();
			AddExpr();
			ParseList.insert(position,L"(LSHIFT " );
			ParseList.append(L") " );
		} else {
			Get();
			AddExpr();
			ParseList.insert(position,L"(RSHIFT " );
			ParseList.append(L") " );
		}
	}
}


void Parser::AddExpr()
{
	int position;
	position=ParseList.length();
	MultExpr();
	while (la->kind == 57 || la->kind == 58) {
		if (la->kind == 57) {
			Get();
			MultExpr();
			ParseList.insert(position,L"(ADD " );
			ParseList.append(L") " );
		} else {
			Get();
			MultExpr();
			ParseList.insert(position,L"(SUB " );
			ParseList.append(L") " );
		}
	}
}


void Parser::MultExpr()
{
	int position;
	position=ParseList.length();
	UnaryExpr();
	while (la->kind == 59 || la->kind == 60 || la->kind == 61) {
		if (la->kind == 59) {
			Get();
			UnaryExpr();
			ParseList.insert(position,L"(MUL " );
			ParseList.append(L") " );
		} else if (la->kind == 60) {
			Get();
			UnaryExpr();
			ParseList.insert(position,L"(DIV " );
			ParseList.append(L") " );
		} else {
			Get();
			UnaryExpr();
			ParseList.insert(position,L"(MOD " );
			ParseList.append(L") " );
		}
	}
}


void Parser::UnaryExpr()
{
	int position;
	position=ParseList.length();
	switch (la->kind) {
	case 1: case 3: case 4: case 5: case 6: case 12: {
		PostfixExpr();
		break;
	}
	case 48: {
		Get();
		ParseList.insert(position,L"(ADDROF ");
		UnaryExpr();
		ParseList.append(L")");
		break;
	}
	case 59: {
		Get();
		ParseList.insert(position,L"(PTR ");
		UnaryExpr();
		ParseList.append(L")");
		break;
	}
	case 57: {
		Get();
		ParseList.insert(position,L"(UPLUS ");
		UnaryExpr();
		ParseList.append(L")");
		break;
	}
	case 58: {
		Get();
		ParseList.insert(position,L"(UMINUS ");
		UnaryExpr();
		ParseList.append(L")");
		break;
	}
	case 62: {
		Get();
		ParseList.insert(position,L"(UNEG ");
		UnaryExpr();
		ParseList.append(L")");
		break;
	}
	case 63: {
		Get();
		ParseList.insert(position,L"(UNOT ");
		UnaryExpr();
		ParseList.append(L")");
		break;
	}
	case 64: {
		Get();
		ParseList.insert(position,L"(PREINC ");
		UnaryExpr();
		ParseList.append(L")");
		break;
	}
	case 65: {
		Get();
		ParseList.insert(position,L"(PREDEC ");
		UnaryExpr();
		ParseList.append(L")");
		break;
	}
	default: SynErr(74); break;
	}
}


void Parser::PostfixExpr()
{
	int position;
	position=ParseList.length();
	Primary();
	while (StartOf(10)) {
		if (la->kind == 8) {
			Get();
			Expression();
			Expect(9);
			ParseList.insert(position,L"(INDEX " );
			ParseList.append(L") " );
		} else if (la->kind == 12) {
			Get();
			if (StartOf(11)) {
				ArgExprList();
			}
			Expect(13);
			ParseList.insert(position,L"(FUNCCALL " );
			ParseList.append(L") " );
		} else if (la->kind == 64) {
			Get();
			ParseList.insert(position,L"(POSTINC " );
			ParseList.append(L") " );
		} else {
			Get();
			ParseList.insert(position,L"(POSTDEC " );
			ParseList.append(L") " );
		}
	}
}


void Parser::Primary()
{
	Name name; int position; 
	if (la->kind == 1) {
		Ident(name);
		ParseList.append(L"(VAR " );ParseList.append(name );ParseList.append(L")" );
	} else if (StartOf(3)) {
		ConstVal();
	} else if (la->kind == 12) {
		Get();
		Expression();
		Expect(13);
	} else SynErr(75);
}


void Parser::ArgExprList()
{
	int position;
	position=ParseList.length();
	AssignExpr();
	ParseList.insert(position,L"(ARG " );
	ParseList.append(L") " );
	while (la->kind == 10) {
		Get();
		position=ParseList.length();
		AssignExpr();
		ParseList.insert(position,L"(ARG " );
		ParseList.append(L") " );
	}
}



void Parser::Parse()
{
	t = NULL;
	// might call Parse() twice
	if (dummyToken) {
		coco_string_delete(dummyToken->val);
		delete dummyToken;
	}
	dummyToken = new Token(coco_string_create(L"Dummy Token"));
	la = dummyToken;
	Get();
	B();
	Expect(0);  // expect end-of-file automatically added
}


Parser::Parser(Scanner* scan, Errors* err)
:
	dummyToken(NULL),
	deleteErrorsDestruct_(!err),
	errDist(minErrDist),
	scanner(scan),
	errors(err),
	t(NULL),
	la(NULL)
{
	if (!errors)    // add in default error handling
	{
		errors = new Errors();
	}
	// user-defined initializations:
}


bool Parser::StartOf(int s)
{
	const bool T = true;
	const bool x = false;

	static const bool set[12][68] =
	{
		{T,T,T,T, T,T,T,x, x,x,x,T, T,x,T,T, T,T,x,T, T,T,x,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,T,T,T, x,x,T,T, T,T,x,x},
		{x,T,x,T, T,T,T,x, T,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,T,x,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{T,T,T,T, T,T,T,x, x,x,x,T, T,x,T,T, T,T,x,T, T,T,x,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,T,T,T, x,x,T,T, T,T,x,x},
		{x,x,T,x, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{T,T,T,T, T,T,T,x, x,x,x,T, T,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,T,T,T, x,x,T,T, T,T,x,x},
		{x,T,T,T, T,T,T,x, x,x,x,T, T,x,T,T, T,T,x,T, T,T,x,T, T,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,T,T,T, x,x,T,T, T,T,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x},
		{x,x,x,x, x,x,x,x, T,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x},
		{x,T,x,T, T,T,T,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,T,T,T, x,x,T,T, T,T,x,x}
	};

	return set[s][la->kind];
}


Parser::~Parser()
{
	if (deleteErrorsDestruct_) { delete errors; } // delete default error handling
	if (dummyToken) {
		coco_string_delete(dummyToken->val);
		delete dummyToken;
	}
	// user-defined destruction:
}


// ----------------------------------------------------------------------------
// Errors Implementation
// ----------------------------------------------------------------------------

Errors::Errors()
:
	count(0)
{}


Errors::~Errors()
{}


void Errors::clear()
{
	count = 0;
}


std::wstring Errors::strerror(int n)
{
	switch (n) {
		case 0: return L"EOF expected"; break;
		case 1: return L"identifier expected"; break;
		case 2: return L"gotolabel expected"; break;
		case 3: return L"number expected"; break;
		case 4: return L"onumber expected"; break;
		case 5: return L"string expected"; break;
		case 6: return L"char expected"; break;
		case 7: return L"\":\" expected"; break;
		case 8: return L"\"[\" expected"; break;
		case 9: return L"\"]\" expected"; break;
		case 10: return L"\",\" expected"; break;
		case 11: return L"\";\" expected"; break;
		case 12: return L"\"(\" expected"; break;
		case 13: return L"\")\" expected"; break;
		case 14: return L"\"case\" expected"; break;
		case 15: return L"\"default\" expected"; break;
		case 16: return L"\"break\" expected"; break;
		case 17: return L"\"{\" expected"; break;
		case 18: return L"\"}\" expected"; break;
		case 19: return L"\"continue\" expected"; break;
		case 20: return L"\"goto\" expected"; break;
		case 21: return L"\"if\" expected"; break;
		case 22: return L"\"else\" expected"; break;
		case 23: return L"\"return\" expected"; break;
		case 24: return L"\"switch\" expected"; break;
		case 25: return L"\"while\" expected"; break;
		case 26: return L"\"extrn\" expected"; break;
		case 27: return L"\"auto\" expected"; break;
		case 28: return L"\"=\" expected"; break;
		case 29: return L"\"=*\" expected"; break;
		case 30: return L"\"=/\" expected"; break;
		case 31: return L"\"=%\" expected"; break;
		case 32: return L"\"=+\" expected"; break;
		case 33: return L"\"=-\" expected"; break;
		case 34: return L"\"=&\" expected"; break;
		case 35: return L"\"=^\" expected"; break;
		case 36: return L"\"=|\" expected"; break;
		case 37: return L"\"=<<\" expected"; break;
		case 38: return L"\"=>>\" expected"; break;
		case 39: return L"\"=<\" expected"; break;
		case 40: return L"\"=<=\" expected"; break;
		case 41: return L"\"=>\" expected"; break;
		case 42: return L"\"=>=\" expected"; break;
		case 43: return L"\"===\" expected"; break;
		case 44: return L"\"=!=\" expected"; break;
		case 45: return L"\"?\" expected"; break;
		case 46: return L"\"|\" expected"; break;
		case 47: return L"\"^\" expected"; break;
		case 48: return L"\"&\" expected"; break;
		case 49: return L"\"==\" expected"; break;
		case 50: return L"\"!=\" expected"; break;
		case 51: return L"\"<\" expected"; break;
		case 52: return L"\">\" expected"; break;
		case 53: return L"\"<=\" expected"; break;
		case 54: return L"\">=\" expected"; break;
		case 55: return L"\"<<\" expected"; break;
		case 56: return L"\">>\" expected"; break;
		case 57: return L"\"+\" expected"; break;
		case 58: return L"\"-\" expected"; break;
		case 59: return L"\"*\" expected"; break;
		case 60: return L"\"/\" expected"; break;
		case 61: return L"\"%\" expected"; break;
		case 62: return L"\"~\" expected"; break;
		case 63: return L"\"!\" expected"; break;
		case 64: return L"\"++\" expected"; break;
		case 65: return L"\"--\" expected"; break;
		case 66: return L"??? expected"; break;
		case 67: return L"this symbol not expected in Definition"; break;
		case 68: return L"invalid Definition"; break;
		case 69: return L"invalid ConstVal"; break;
		case 70: return L"invalid Initializator"; break;
		case 71: return L"this symbol not expected in Statement"; break;
		case 72: return L"invalid Statement"; break;
		case 73: return L"invalid Label"; break;
		case 74: return L"invalid UnaryExpr"; break;
		case 75: return L"invalid Primary"; break;
		default:
		{
			// std::wostringstream buf;  (this typedef might be missing)
			std::basic_ostringstream<wchar_t> buf;
			buf << "error " << n;
			return buf.str();
		}
		break;
	}
}


void Errors::Warning(const std::wstring& msg)
{
	fwprintf(stderr, L"%ls\n", msg.c_str());
}


void Errors::Warning(int line, int col, const std::wstring& msg)
{
	fwprintf(stderr, L"-- line %d col %d: %ls\n", line, col, msg.c_str());
}


void Errors::Error(int line, int col, const std::wstring& msg)
{
	fwprintf(stderr, L"-- line %d col %d: %ls\n", line, col, msg.c_str());
	count++;
}


void Errors::SynErr(int line, int col, int n)
{
	this->Error(line, col, this->strerror(n));
}


void Errors::Exception(const std::wstring& msg)
{
	fwprintf(stderr, L"%ls", msg.c_str());
	::exit(1);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //


// ************************************************************************* //
