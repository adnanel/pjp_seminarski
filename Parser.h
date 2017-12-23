// This file was generated with Coco/R C++, version: 20101106
// http://www.ssw.uni-linz.ac.at/coco/
// with these defines:


#ifndef COCO_PARSER_H__
#define COCO_PARSER_H__

using namespace std;
#include <wchar.h>
typedef  wchar_t Name[500];
#include <string>
#include <sstream>
#include <map>
#include <vector>

#include <iostream>


#include "Scanner.h"



/*---------------------------------------------------------------------------*\
                           Class Errors Declaration
\*---------------------------------------------------------------------------*/
//! Parser error handling
class Errors
{
public:
	int count;      //!< The number of errors detected

	//! Return a string describing the given error code.
	static std::wstring strerror(int n);

	Errors();               //!< Construct null - start with no errors
	virtual ~Errors();      //!< Destructor
	virtual void clear();   //!< Clear the error count

	//! Handle a general warning 'msg'
	virtual void Warning(const std::wstring& msg);
	//! Handle a general warning 'msg'
	virtual void Warning(int line, int col, const std::wstring& msg);
	//! Handle general error 'msg' (eg, a semantic error)
	virtual void Error(int line, int col, const std::wstring& msg);
	//! Handle syntax error 'n', uses strerror for the message, calls Error()
	virtual void SynErr(int line, int col, int n);
	//! Handle a general exception 'msg'
	virtual void Exception(const std::wstring& msg);

}; // Errors



/*---------------------------------------------------------------------------*\
                           Class Parser Declaration
\*---------------------------------------------------------------------------*/
//! A Coco/R Parser
class Parser
{
	enum {
		_EOF=0,
		_identifier=1,
		_gotolabel=2,
		_number=3,
		_onumber=4,
		_string=5,
		_char=6,
		maxT = 66    //<! max term (w/o pragmas)
	};
	static const int minErrDist = 2; //!< min. distance before reporting errors

	Token *dummyToken;
	bool deleteErrorsDestruct_; //!< delete the 'errors' member in destructor
	int  errDist;

	void SynErr(int n);         //!< Handle syntax error 'n'
	void Get();
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Errors  *errors;

	Token *t;                   //!< last recognized token
	Token *la;                  //!< lookahead token

#include "methods.cpp"

#define PRINT_PARSE_LIST

std::wstring ParseList;
int scopepos;


	//! Construct for the specified scanner
	/*!
	 * Use the default error handling, or optionally provide an error
	 * handler, which will not be deleted upon destruction.
	 */
	Parser(Scanner* scan, Errors* err = 0);
	~Parser();
	void Parse();                          //!< Execute the parse operation
	void SemErr(const std::wstring& msg);  //!< Handle semantic error
	bool isUTF8() const;   //!< Return true if scanner buffer is UTF8

	void B();
	void Definition();
	void Ident(Name name);
	void GotoLabel(Name name);
	void ConstVal();
	void FunctionDefinition();
	void ArraySize(bool & isarray);
	void Initializator();
	void FunctionHeader();
	void FunctionBody();
	void FormalParamList();
	void Statement();
	void FormalParameter();
	void Label();
	void ExtrnDeclaration();
	void AutoDeclaration();
	void StatementExpression();
	void BreakStatement();
	void CompoundStatement();
	void ContinueStatement();
	void GotoStatement();
	void IfStatement();
	void NullStatement();
	void ReturnStatement();
	void SwitchStatement();
	void WhileStatement();
	void Expression();
	void AssignExpr();
	void CondExpr();
	void OrExpr();
	void XorExpr();
	void AndExpr();
	void EqlExpr();
	void RelExpr();
	void ShiftExpr();
	void AddExpr();
	void MultExpr();
	void UnaryExpr();
	void PostfixExpr();
	void Primary();
	void ArgExprList();

}; // end Parser


#endif // COCO_PARSER_H__

// ************************************************************************* //
