/* Implementation of Recursive-Descent Parser
 * parse.cpp
 * Programming Assignment 2
 * Fall 2022
*/

/*Questions: 
	How do you increment the memory reference to linenumber ( int& line)
*/


#include "lex.cpp"
#include "parseInt.h"
#include "val.h"


map<string, Token> SymTable;
map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects


map<string, bool> defVar;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream& in, int& line);

bool Prog(istream& in, int& line) {
	bool status;
	LexItem t = Parser::GetNextToken(in, line);
	
	if(t.GetToken() == PROGRAM){
		//cout << "Program" << endl;
		 t= Parser::GetNextToken(in, line);
	}
	else{
		ParseError(line, "No PROGRAM at start of file");
		return false;
	}
	if(t.GetToken() != IDENT){
		ParseError(line, "Missing file name");
		return false;
	}
	//line++;
	//cout << "First instance in statement list, lexeme: " << t.GetLexeme() << endl;
	status = StmtList(in, line);
	if(!status){
		ParseError(line, "Incorrect Program Body, Prog");
		return status;
	}

	t = Parser::GetNextToken(in, line);
	//cout << t.GetLexeme() << endl;
	if(t.GetToken() != END){
		ParseError(line, "Missing END at bottom of file");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if(t.GetToken() != PROGRAM){
		ParseError(line, "Missing PROGRAM after END at bottom of file");
		return false;
	}

	return true;
}

bool StmtList(istream& in, int& line) {
	bool status;
	LexItem t = Parser::GetNextToken(in, line);
	//cout << t.GetLinenum() <<"STMT List: Lexeme: " << t.GetLexeme() << "; Token: " << t.GetToken() << endl;
	if(t.GetToken() == ELSE || t.GetToken() == END){
		Parser::PushBackToken(t);
		return true;
	}
	Parser::PushBackToken(t);
	
	//How to decide whether statement is a Declarative Statement or Control Statement?
	status = Stmt(in, line);

	if(!status){
		ParseError(line, "Syntax Error in Stmt, StmtList");
		return status;
	}
	//DeclStmt
	t = Parser::GetNextToken(in, line);
	
	if(t.GetToken() == SEMICOL){
		t = Parser::GetNextToken(in, line);
		if(t == END){
			Parser::PushBackToken(t);
			return true;
		}
		else if(t == PROGRAM){
			ParseError(line, "Syntactic Error in program body");
			return false;
		}
		Parser::PushBackToken(t);
		status = StmtList(in, line);
	}
	else if(t.GetToken() != END){
		ParseError(line, "Missing semicolon at end of statement");
		return false;
		//Parser::PushBackToken(t);
	}
	

	return status;
}

bool Stmt(istream& in, int& line){
	bool status;
	LexItem t = Parser::GetNextToken(in, line);
	// cout << "STMT: Lexeme: " << t.GetLexeme() << "; Token: " << t.GetToken() << endl;
	//cout << t.GetLinenum()<<":::STMT: Lexeme: " << t.GetLexeme() << "; Token: " << t.GetToken() << endl;

	if(t.GetToken() == INT || t.GetToken() == FLOAT || t.GetToken() == BOOL){
		Parser::PushBackToken(t);
		status = DeclStmt(in, line);
		if(!status){
			ParseError(line, "Error in DeclStmt, Stmt");
			return false;
		}
	}
	else{
		Parser::PushBackToken(t);
		status = ControlStmt(in, line);
		if(!status){
			ParseError(line, "Error in ControlStmt, Stmt");
			return false;
		}
	}
		
		return status;
}

bool DeclStmt(istream& in, int& line){
	LexItem t = Parser::GetNextToken(in, line);
	bool status;
	if(t.GetToken() == INT || t.GetToken() == FLOAT || t.GetToken() == BOOL){
		status = VarList(in, line);
		if(!status){
			ParseError(line, "Incorrect variable in DeclStmt");
		}
		return status;
	}
	ParseError(line, "Declaration Error");
	return false;
}

bool VarList(istream& in, int& line){
	//LexItem t = Parser::GetNextToken(in, line);
	bool status;
	status = Var(in, line);
	if(!status){
		//ParseError(line, "Unidentified Variable in Variable List");
		return status;
	}

	LexItem t = Parser::GetNextToken(in, line);

	if(t.GetToken() == COMMA){
		status = VarList(in, line);
	}
	else if(t.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << t.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(t);
		return true;
	}

	return status;
}

bool Var(istream& in, int& line){
	LexItem t = Parser::GetNextToken(in, line);
	//cout << "LEXEME: " << t.GetLexeme() << "; Token: " << t.GetToken()<<endl;
	if(t.GetToken() != IDENT || t.GetLexeme() == "REAL"){
		ParseError(line, "Variable is not an identifier");
		return false;
	}
	string identstr;
	identstr = t.GetLexeme();
		if (!(defVar.find(identstr)->second))
		{
			LexItem t = Parser::GetNextToken(in, line);
			if(t.GetToken() == ASSOP){
				ParseError(line, "Undeclared variable");
				return false;
			}
			Parser::PushBackToken(t);
			defVar[identstr] = true;
		}	
		else
		{
			LexItem t2 = Parser::GetNextToken(in, line);
			if(t2.GetToken() == ASSOP){
				Parser::PushBackToken(t2);
				Parser::PushBackToken(t);
				return true;
			}
			ParseError(line, "Variable Redefinition");
			return false;
		}
	return true;
}

// bool ControlStmt(istream& in, int& line){
// 	bool status;
// 	LexItem t = Parser::GetNextToken(in, line);
// 	if(t.GetToken() == PRINT){
// 		status = PrintStmt(in, line);
// 	}
// 	if(t.GetToken() == ASSOP){
// 		status = AssignStmt(in, line);
// 	}
// 	if(t.GetToken() == IF){
// 		status = IfStmt(in, line);
// 	}
// 	return status;
// }

//ControlStmt ::= AssigStmt | IfStmt | PrintStmt 
bool ControlStmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case PRINT:
		status = PrintStmt(in, line);
		break;

	case IF:
		status = IfStmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		break;
		
	default:
		Parser::PushBackToken(t);
		//ParseError(line, "Undeclared variable");
		return false;
	}

	return status;
}//End of ControlStmt


//PrintStmt:= PRINT (ExpreList) 
bool PrintStmt(istream& in, int& line) {
	LexItem t;
	ValQue = new queue<Value>;

	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression list after Print");
		while(!(*ValQue).empty())
		{
			ValQue->pop();		
		}
		delete ValQue;
		return false;
	}

	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;	

	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	return ex;
}//End of PrintStmt

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	status = Expr(in, line);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	
	LexItem tok = Parser::GetNextToken(in, line);
	//cout << tok.GetLinenum() <<":: EXPR List: Lexeme: " << tok.GetLexeme() << "; Token: " << tok.GetToken() << endl;
	
	if (tok == COMMA) {
		status = ExprList(in, line);
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//End of ExprList

bool AssignStmt(istream& in, int& line) {
	bool status;
	status = Var(in, line);
	if(!status){
		ParseError(line, "No lefthand side variable in AssignStmt");
		return status;
	}
		LexItem v = Parser::GetNextToken(in, line);
		LexItem a = Parser::GetNextToken(in, line);

		//LexItem i = Parser::GetNextToken(in, line);
		//cout << t.GetLinenum() << "::: LEXEME: " << t.GetLexeme()<<endl;
		if(a.GetToken() != ASSOP){
			ParseError(line, "No assignment operator");
			return false;
		}
		Value val;
		status = Expr(in, line, val);
		if(status && !val.IsErr()){
			TempsResults[v.GetLexeme()] = val;
		}
		return status;
}

bool IfStmt(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);
	bool status;
	if(tok.GetToken() != LPAREN){
		ParseError(line, "Missing Left Paranthesis");
		return false;
	}
	status = Expr(in, line);

	if(!status){
		return status;
	}

	tok = Parser::GetNextToken(in, line);

	if(tok.GetToken() != RPAREN){
		ParseError(line, "Missing Right Paranethesis");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	//cout << tok.GetLexeme() << endl;

	if(tok.GetToken() != THEN){
		ParseError(line, "Missing THEN in IF statement");
		return false;
	}
	status = StmtList(in, line);
	if(!status){
		ParseError(line, "Error in THEN body");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	//cout << tok.GetLexeme() << endl;

	while(tok.GetToken() == ELSE){
		status = StmtList(in, line);
		if(!status){
			ParseError(line, "Missing else body");
			return status;
		}
		tok = Parser::GetNextToken(in, line);
	}

	// tok = Parser::GetNextToken(in, line);

	// cout << tok.GetLexeme() << endl;

	// if(tok.GetToken() != END){
	// 	ParseError(line, "Missing END to IF statement");
	// 	return false;
	// }
	
	//tok = Parser::GetNextToken(in, line);
	//cout << tok.GetLexeme() << endl;
	if(tok.GetToken() == END){
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() != IF){
			ParseError(line, "Missing IF aftern END of IF statement");
			return false;
		}
		return true;
	}
	if(tok.GetToken() != IF){
		ParseError(line, "Missing IF aftern END of IF statement");
		return false;
	}

	return status;
}

bool Expr(istream& in, int& line) {
	bool status;
	status = LogANDExpr(in, line, Value(true));
	LexItem t = Parser::GetNextToken(in, line);
	//cout << t.GetLinenum() <<": Expr: Lexeme: " << t.GetLexeme() << "; Token: " << t.GetToken() << endl;
	if(t.GetToken() == OR){
		status = Expr(in, line);
		return status;
	}
	else{
		Parser::PushBackToken(t);
	}
	return status;
}
//LogANDExpr ::= EqualExpr { && EqualExpr }
bool LogANDExpr(istream& in, int& line, Value & retVal) {
	LexItem tok;
	Value val1, val2;
	bool t1 = EqualExpr(in, line, val1);
	//cout << "status of EqualExpr and val1: " << t1<< " " << val1.IsErr() << " " << val1.IsBool() << endl;
	//cout << "status of EqualExpr: " << t1<< endl;
		
	if( !t1 ) {
		return false;
	}
	//cout << "value of var1 in AND op " << val1.GetBool() << endl;
	retVal = val1;
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == AND ) 
	{
		t1 = EqualExpr(in, line, val2);
		//cout << "value of var1 in AND op " << val1.GetBool() << endl;
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		//evaluate expression for Logical AND
		retVal = retVal && val2;
		//cout << "AND op result " << retVal.IsBool() << " " << retVal.GetBool() << endl;
		if(retVal.IsErr())
		{
			ParseError(line, "Illegal AND operation.");
			//cout << "(" << tok.GetLexeme() << ")" << endl;		
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
	}
	
	Parser::PushBackToken(tok);
	return true;
}//End of LogANDExpr

//OLD LogANDExpr from PA2

// bool LogANDExpr(istream& in, int& line, Value & retVal) {
// 	bool status;
// 	status = EqualExpr(in, line);
// 	if(!status){
// 		ParseError(line, "Logical expression missing equals");
// 		return status;
// 	}
// 	LexItem t = Parser::GetNextToken(in, line);
// 	if(t.GetToken() == AND){
// 		status = LogANDExpr(in, line);
// 		return status;
// 	}
// 	Parser::PushBackToken(t);
// 	return status;
// }

bool EqualExpr(istream& in, int& line, Value & retVal) {
	bool status;
	status = RelExpr(in, line, retVal);
	if(!status){
		ParseError(line, "Real expression missing in EqualExpr");
		return status;
	}
	LexItem t = Parser::GetNextToken(in, line);
	if(t.GetToken() == EQUAL){
		status = RelExpr(in, line);
		return status;
	}
	Parser::PushBackToken(t);
	return status;
}

bool RelExpr(istream& in, int& line, Value & retVal) {
	bool status;
	status = AddExpr(in, line);
	if(!status){
		ParseError(line, "Add error in RelExpr");
		return status;
	}
	LexItem t = Parser::GetNextToken(in, line);
	if(t.GetToken() == GTHAN || t.GetToken() == LTHAN){
		status = AddExpr(in, line);
	}
	else{
		Parser::PushBackToken(t);
	}

	return status;
}

bool AddExpr(istream& in, int& line, Value & retVal) {
	bool status;
	status = MultExpr(in, line);
	if(!status){
		ParseError(line, "Incorrect MultExpr in AddExpr");
		return status;
	}
	LexItem t = Parser::GetNextToken(in, line);
	if(t.GetToken() == PLUS || t.GetToken() == MINUS){
		status = AddExpr(in, line);
	}
	else{
		Parser::PushBackToken(t);
	}
	return status;
}

bool MultExpr(istream& in, int& line, Value & retVal) {
	bool status;
	status = UnaryExpr(in, line);
	if(!status){
		ParseError(line, "Incorrect UnaryExpr in MultExpr");
		return status;
	}
	LexItem t = Parser::GetNextToken(in, line);
	if(t.GetToken() == MULT || t.GetToken() == DIV){
		status = MultExpr(in, line);
	}
	else{
		Parser::PushBackToken(t);
	}
	return status;
}

bool UnaryExpr(istream& in, int& line, Value & retVal) {
	bool status;
	LexItem t = Parser::GetNextToken(in, line);
	//cout << t.GetLexeme() << endl;
	if(t.GetToken() == MINUS || t.GetToken() == PLUS || t.GetToken() == NOT){
		if(t.GetToken() == MINUS){
		status = PrimaryExpr(in, line, 0);
		}
		else if(t.GetToken() == PLUS){
			status = PrimaryExpr(in, line, 1);
		}
		else{
			status = PrimaryExpr(in, line, 2);
		}
		if(!status){
			ParseError(line, "PrimraryExpr in UnaryExpr");
		}
		return status;
	}
	else{
		Parser::PushBackToken(t);
		status = PrimaryExpr(in, line, 0);
	}
	//ParseError(line, "No sign in UnaryExpr");
	return status;

}

bool PrimaryExpr(istream& in, int& line, int sign){
	LexItem t = Parser::GetNextToken(in, line);
	if(t.GetToken() == IDENT ){
		string lexeme = t.GetLexeme();
		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undefined Variable");
			return false;	
		}
		return true;
	}
	else if(t.GetToken() == ICONST || t.GetToken() == RCONST || t.GetToken() == SCONST || t.GetToken() == BCONST){
		return true;
	}
	else if( t.GetToken() == LPAREN ) {
		bool ex = Expr(in, line);
		if( !ex ) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		LexItem t = Parser::GetNextToken(in, line);
		// cout << t.GetLexeme() << endl;
		if( t.GetToken() == RPAREN ){
			//Parser::PushBackToken(t);
			return ex;
		}
		else 
		{
			Parser::PushBackToken(t);
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}
	else if(t.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << t.GetLexeme() << ")" << endl;
		return false;
	}

	return false;
}
