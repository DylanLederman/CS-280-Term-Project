/* Implementation of Recursive-Descent Parser
 * parseInt.cpp
 * Programming Assignment 3
 * Fall 2022
*/

#include "parseInt.h"

map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects

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

	status = StmtList(in, line);
	if(!status){
		ParseError(line, "Incorrect Program Body, Prog");
		return status;
	}

	t = Parser::GetNextToken(in, line);

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
	//cout << "Statment list after call to statement: " << t.GetLexeme() << endl;
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
		status = VarList(in, line, t);
		if(!status){
			ParseError(line, "Incorrect variable in DeclStmt");
		}
		return status;
	}
	ParseError(line, "Declaration Error");
	return false;
}


bool VarList(istream& in, int& line, LexItem& type){
	LexItem t = Parser::GetNextToken(in, line);
	Parser::PushBackToken(t);
	bool status;
	status = Var(in, line, type);
	if(!status){
		//ParseError(line, "Unidentified Variable in Variable List");
		return status;
	}

	 t = Parser::GetNextToken(in, line);

	if(t.GetToken() == COMMA){
		LexItem t1 = Parser::GetNextToken(in, line);
		Parser::PushBackToken(t1);
		status = VarList(in, line, type);
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
	//cout << line << ": PrintStmt: " << t.GetLexeme() << endl;
	ValQue = new queue<Value>;
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Incorrect expression list after Print");
		while(!(*ValQue).empty())
		{
			ValQue->pop();		
		}
		delete ValQue;
		return false;
	}
	
	//Evaluate: print out the list of expressions' values
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
	
	return true;
}//End of PrintStmt





bool IfStmt(istream& in, int& line) {
	LexItem tok = Parser::GetNextToken(in, line);
	bool status;
	if(tok.GetToken() != LPAREN){
		ParseError(line, "Missing Left Paranthesis");
		return false;
	}
	Value valRef;
	status = Expr(in, line, valRef);

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
	if(valRef.GetBool()){
	status = StmtList(in, line);
	tok = Parser::GetNextToken(in, line);
	}
	if(!status){
		ParseError(line, "Error in THEN body");
		return false;
	}

	while(tok.GetToken() != ELSE && tok.GetToken() != END){
		tok = Parser::GetNextToken(in, line);
	}
	
	//cout << tok.GetLexeme() << endl;
	//cout << valRef.GetType() << valRef.GetBool() << endl;
	if(tok.GetToken() == ELSE){
		if(!valRef.GetBool()){
			status = StmtList(in, line);
			if(!status){
				ParseError(line, "Missing else body");
				return status;
			}
			tok = Parser::GetNextToken(in, line);
		}
		else{
			// cout << "WE out here" << endl;
			while(tok.GetLexeme() != "END"){
				tok = Parser::GetNextToken(in, line);
				// cout << tok.GetLexeme() << endl;
				// cout << tok.GetToken() << endl;
			}
		}
	}
	// cout << tok.GetLexeme() << endl;
	// cout << tok.GetToken() << endl;
	if(tok.GetToken() == END){
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() != IF){
			ParseError(line, "Missing IF aftern END of IF statement");
			return false;
		}
		return true;
	}
	else{
		ParseError(line, "Missing END after IFstatement");
		return false;
	}
	

	return status;
}


bool AssignStmt(istream& in, int& line) {
	bool status;
	LexItem t = Parser::GetNextToken(in, line);
	Parser::PushBackToken(t);
	//cout << "LexItem in AssignmentStatement: " << t.GetLexeme() << endl;
	status = Var(in, line, t);
	if(!status){
		ParseError(line, "No lefthand side variable in AssignStmt");
		return status;
	}
		//LexItem v = Parser::GetNextToken(in, line);
		LexItem a = Parser::GetNextToken(in, line);


		if(a.GetToken() != ASSOP){
			ParseError(line, "No assignment operator");
			return false;
		}
		Value val;
		status = Expr(in, line, val);
		//cout << endl <<"Assigning values in TempsResults, Identifier: " << t.GetLexeme() << ", with type: "<< SymTable[t.GetLexeme()] << ", to value with value type: " << val.GetType() << endl;
		if(status && !val.IsErr()){
			//Here is where you can worry about assigning indentifiers to values with the right data types
			if(SymTable[t.GetLexeme()] == INT){
				if(!val.IsInt()){
					if(val.IsReal()){
						float aux = val.GetReal();
						val.SetType(VINT);
						val.SetInt((int)aux);
						TempsResults[t.GetLexeme()] = val;
					}
					else{
						ParseError(line, "Incompatible datatype with INT");
						return false;
					}
				}
				TempsResults[t.GetLexeme()] = val;
			}
			else if(SymTable[t.GetLexeme()] == FLOAT){
				if(val.IsReal()){
					TempsResults[t.GetLexeme()] = val;
				}
				else if(val.IsInt()){
					float aux = (float)val.GetInt();
					val.SetType(VREAL);
					val.SetReal(aux);
					TempsResults[t.GetLexeme()] = val;
				}
			}
			else if(SymTable[t.GetLexeme()] != BOOL || !val.IsBool() ){
				ParseError(line, "Incompatible assignment");
				return false;
			}
			else{
			TempsResults[t.GetLexeme()] = val;
			}
		}
		if(!status){
			ParseError(line, "Invalid Expression");
			return false;
		}
		return status;
}


bool Var(istream& in, int& line, LexItem& idtok){
	LexItem t = Parser::GetNextToken(in, line);
	//cout << "LEXEME: " << t.GetLexeme() << "; Token: " << t.GetToken()<<endl;
	if(t.GetToken() != IDENT || t.GetLexeme() == "REAL"){
		ParseError(line, "Variable is not an identifier");
		return false;
	}
	string identstr;
	identstr = t.GetLexeme();
		if (!defVar[identstr])
		{
			//cout << "Not in defVar" << endl;
			LexItem t1 = Parser::GetNextToken(in, line);
			if(t1.GetToken() == ASSOP){
				ParseError(line, "Undeclared variable");
				return false;
			}
			Parser::PushBackToken(t1);
			defVar[identstr] = true;
			SymTable[t.GetLexeme()] = idtok.GetToken();
		}	
		else
		{
			LexItem t2 = Parser::GetNextToken(in, line);
			if(t2.GetToken() == ASSOP){
				Parser::PushBackToken(t2);
				return true;
			}
			ParseError(line, "Variable Redefinition");
			return false;
		}
	return true;
}


//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value valRef;
	status = Expr(in, line, valRef);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	if(valRef.IsErr()){
		ParseError(line, "Error in first item in Expression List");
		return false;
	}
	(*ValQue).push(valRef);

	
	LexItem tok = Parser::GetNextToken(in, line);
	//cout << tok.GetLinenum() <<":: EXPR List: Lexeme: " << tok.GetLexeme() << "; Token: " << tok.GetToken() << endl;
	
	while (tok == COMMA) {
		valRef = new Value();
		status = Expr(in, line, valRef);
		if(!status){
			ParseError(line, "Missing Expression");
			return false;
		}
		if(valRef.IsErr()){
		ParseError(line, "Error in other items in Expression List");
		ParseError(line, "UNDECLARED VARIABLE");
		return false;
	}
		(*ValQue).push(valRef);
		tok = Parser::GetNextToken(in, line);
	}
	if(tok.GetToken() == ERR){
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

bool Expr(istream& in, int& line, Value & retVal) {
	LexItem tok;

	Value val1, val2;
	bool t1 = LogANDExpr(in, line, val1);
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
	while ( tok == OR) 
	{
		t1 = LogANDExpr(in, line, val2);
		//cout << "value of var1 in AND op " << val1.GetBool() << endl;
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		//evaluate expression for Logical AND
		retVal = retVal || val2;
		//cout << "AND op result " << retVal.IsBool() << " " << retVal.GetBool() << endl;
		if(retVal.IsErr())
		{
			ParseError(line, "Illegal or operation.");
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
//LogANDExpr ::= EqualExpr { && EqualExpr }

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
		//cout << "retVal status: " << retVal.GetBool() << ", val2 status: " << val2.GetBool();
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


bool EqualExpr(istream& in, int& line, Value & retVal) {
	bool status;
	Value val1, val2;
	status = RelExpr(in, line, val1);
	if(!status){
		//ParseError(line, "Real expression missing in EqualExpr");
		return status;
	}
	retVal = val1;
	LexItem t = Parser::GetNextToken(in, line);
	if(t.GetToken() == EQUAL){
		status = RelExpr(in, line, val2);
		if(!status){
			ParseError(line, "Missing operand equality operator");
			return false;
		}
		retVal = retVal == val2;
		if(retVal.IsErr())
		{
			ParseError(line, "Illegal eaulity operation.");
			//cout << "(" << tok.GetLexeme() << ")" << endl;		
			return false;
		}
		return status;
	}
	
	Parser::PushBackToken(t);
	return status;
}

bool RelExpr(istream& in, int& line, Value & retVal) {
	bool status;
	Value val1, val2;
	status = AddExpr(in, line, val1);
	if(!status){
		//ParseError(line, "Add error in RelExpr");
		return status;
	}
	retVal = val1;
	LexItem t = Parser::GetNextToken(in, line);
	if(t.GetToken() == GTHAN || t.GetToken() == LTHAN){
		status = AddExpr(in, line, val2);
		if(!status){
			ParseError(line, "Missing operator after gthan/lthan");
			return false;
		}
		if(t.GetToken() == GTHAN){
			//cout << "Val1, Val2" << endl;
			// cout << retVal.GetType() << ", " << val2.GetType() <<endl;
			// cout << retVal.GetInt() <<", "<< val2.GetInt() << endl;
			retVal = retVal > val2;
		}
		else{
			retVal = retVal < val2;
		}
			if(retVal.IsErr())
		{
			ParseError(line, "Illegal GTHAN/LTHAN operation.");
			//cout << "(" << tok.GetLexeme() << ")" << endl;		
			return false;
		}
	}

	else{
		Parser::PushBackToken(t);
	}

	return status;
}

bool AddExpr(istream& in, int& line, Value & retVal) {
	bool status;
	Value val1, val2;
	status = MultExpr(in, line, val1);
	if(!status){
		//ParseError(line, "Incorrect MultExpr in AddExpr");
		return status;
	}
	retVal = val1;
	LexItem t = Parser::GetNextToken(in, line);
	while(t.GetToken() == PLUS || t.GetToken() == MINUS){
		status = MultExpr(in, line, val2);
		if(!status){
			ParseError(line, "Illegal addition expression");
			return false;
		}
		retVal = retVal + val2;
		t = Parser::GetNextToken(in, line);
	}
		Parser::PushBackToken(t);
	return status;
}

bool MultExpr(istream& in, int& line, Value & retVal) {
	bool status;
	Value val1, val2;
	status = UnaryExpr(in, line, val1);
	if(!status){
		//ParseError(line, "Incorrect UnaryExpr in MultExpr");
		return status;
	}
	retVal = val1;
	LexItem t = Parser::GetNextToken(in, line);
	while(t.GetToken() == MULT || t.GetToken() == DIV){
		status = MultExpr(in, line, val2);
		if(!status){
			ParseError(line, "Illegal operand after Mult/Div operator");
			return false;
		}
		if(t.GetToken() == MULT){
			retVal = retVal*val2;
		}
		else{
			if(val2.IsInt()){
				if(val2.GetInt() == 0){
					ParseError(line, "Division by 0");
					return false;
				}
			}
			else if(val2.GetReal() == 0){
					ParseError(line, "Division by 0");
					return false;
				}
			retVal = retVal/val2;
		}
		t = Parser::GetNextToken(in, line);
	}
		Parser::PushBackToken(t);
	
	return status;
}

bool UnaryExpr(istream& in, int& line, Value & retVal) {
	bool status;
	LexItem t = Parser::GetNextToken(in, line);
	Value val;
	//cout << t.GetLexeme() << endl;
	if(t.GetToken() == MINUS || t.GetToken() == PLUS || t.GetToken() == NOT){
		if(t.GetToken() == MINUS){
		status = PrimaryExpr(in, line, 0, val);
		}
		else if(t.GetToken() == PLUS){
			status = PrimaryExpr(in, line, 1, val);
		}
		else{
			//cout << endl << "Not found, haha get it?" << endl;
			status = PrimaryExpr(in, line, 2, val);
		}
		if(!status){
			//ParseError(line, "PrimraryExpr in UnaryExpr");
			return false;
		}
	}
	else{
		Parser::PushBackToken(t);
		status = PrimaryExpr(in, line, 1, val);
	}
	if(status){
		retVal = val;
	}
	//ParseError(line, "No sign in UnaryExpr");
	return status;

}

bool PrimaryExpr(istream& in, int& line, int sign, Value & retVal){
	LexItem t = Parser::GetNextToken(in, line);
	if(t.GetToken() == IDENT ){
		string lexeme = t.GetLexeme();
		// if (TempsResults[lexeme].IsErr())
		// {
		// 	ParseError(line, "Using Undefined Variable");
		// 	return false;	
		// }
		 retVal = TempsResults[lexeme];
		// cout << sign << endl;
		// cout << "Primary Expression Lexeme: " << lexeme << endl;
		// cout << "Value type in Primary Expression: " << retVal.GetType() << endl;
		if(sign == 0 && retVal.GetType() == 3){
			ParseError(line, "Cannot assign minus to boolean");
			return false;
		}
		if(sign == 2)
		{
			if(retVal.GetType() != VBOOL){
				ParseError(line, "Illegal operand after not operator");
				return false;
			}
			else{
				retVal.SetBool(!retVal.GetBool());
			}
		}
		return true;
	}
	if(t.GetToken() == BCONST || t.GetToken() == BOOL){
		retVal.SetType(VBOOL);

		if(t.GetLexeme() == "TRUE" || t.GetLexeme() == "true"){
			if(sign == 2){
				retVal = Value(false);
			}
			else{
				retVal = Value(true);
			}
		}
		else{
			if(sign == 2){
				retVal = Value(true);
			}
			else{
				retVal = Value(false);
			}
		}
		
		return true;
	}
	else if(t.GetToken() == ICONST || t.GetToken() == RCONST || t.GetToken() == SCONST){
		retVal = Value(t.GetLexeme());
		if(t.GetToken() == SCONST){
			retVal.SetType(VSTRING);
			retVal.SetString(t.GetLexeme());
			if(sign == 2){
				retVal = Value();
				ParseError(line, "Cannont evaluate ! on integer, real, or string");
				return false;
			}
		}
		if(t.GetToken() == ICONST){
			retVal.SetType(VINT);
			retVal.SetInt(stoi(t.GetLexeme()));
			if(sign == 0){
				retVal.SetInt(0 - retVal.GetInt());
			}
		}
		if(t.GetToken() == RCONST){
			retVal.SetType(VREAL);
			retVal.SetReal(stof(t.GetLexeme()));
			if(sign == 0){
				retVal.SetReal(0 - retVal.GetReal());
			}
		}
		//cout << "Return Value Type: " << retVal.GetType() << endl;
		return true;
	}

	else if( t.GetToken() == LPAREN ) {
		Value val;
		bool ex = Expr(in, line, val);
		if( !ex ) {
			ParseError(line, "Missing expression after (");
			return false;
		}
		retVal = val;
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




