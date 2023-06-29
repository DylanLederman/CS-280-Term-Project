#ifndef VALUE_H
#define VALUE_H

#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <iomanip>

using namespace std;

enum ValType { VINT, VREAL, VSTRING, VBOOL, VERR };

class Value {
    ValType	T;
    bool    Btemp;
	int		Itemp;
	float   Rtemp;
    string	Stemp;
       
public:
    Value() : T(VERR), Btemp(false), Itemp(0), Rtemp(0.0), Stemp("") {}
    Value(bool vb) : T(VBOOL), Btemp(vb), Itemp(0), Rtemp(0.0), Stemp("") {}
    Value(int vi) : T(VINT), Btemp(false), Itemp(vi), Rtemp(0.0), Stemp("") {}
	Value(float vr) : T(VREAL), Btemp(false), Itemp(0), Rtemp(vr), Stemp("") {}
    Value(string vs) : T(VSTRING), Btemp(false), Itemp(0), Rtemp(0.0), Stemp(vs) {}
    
    
    ValType GetType() const { return T; }
    bool IsErr() const { return T == VERR; }
    bool IsInt() const { return T == VINT; }
    bool IsString() const { return T == VSTRING; }
    bool IsReal() const {return T == VREAL;}
    bool IsBool() const {return T == VBOOL;}
    
    int GetInt() const { if( IsInt() ) return Itemp; throw "RUNTIME ERROR: Value not an integer"; }
    
    string GetString() const { if( IsString() ) return Stemp; throw "RUNTIME ERROR: Value not a string"; }
    
    float GetReal() const { if( IsReal() ) return Rtemp; throw "RUNTIME ERROR: Value not an integer"; }
    
    bool GetBool() const {if(IsBool()) return Btemp; throw "RUNTIME ERROR: Value not a boolean";}
    
    void SetType(ValType type)
    {
    	T = type;
	}
	
    void SetInt(int val)
    {
    	Itemp = val;
	}
	
	void SetReal(float val)
    {
    	Rtemp = val;
	}
	
	void SetString(string val)
    {
    	Stemp = val;
	}
	
	void SetBool(bool val)
    {
    	Btemp = val;
	}
	
	
    // add op to this
 // add op to this
    Value operator+(const Value& op)const{
        if(op.IsErr() || T == VERR){
            return Value();
        }
        if(op.IsInt()){
            if(T == VINT){
                int sub = GetInt() + op.GetInt();
                return Value(sub);
            }
            else if(T == VREAL){
                float sub = GetReal() + op.GetInt();
                return Value(sub);
            }
            else{
                return Value();
            }
            
        }

        else if(op.IsReal()){
            if(T == VINT){
                float sub = GetInt() + op.GetReal();
                return Value(sub);
            }
            else if(T == VREAL){
                float sub = GetReal() + op.GetReal();
                return Value(sub);
            }
            else{
                return Value();
            }
        }
        return Value();
    };
    
    // subtract op from this
    Value operator-(const Value& op)const {
        if(op.IsErr() || T == VERR){
            return Value();
        }
        if(op.IsInt()){
            if(T == VINT){
                int sub = GetInt() - op.GetInt();
                return Value(sub);
            }
            else if(T == VREAL){
                float sub = GetReal() - op.GetInt();
                return Value(sub);
            }
            else{
                return Value();
            }
            
        }

        else if(op.IsReal()){
            if(T == VINT){
                float sub = GetInt() - op.GetReal();
                return Value(sub);
            }
            else if(T == VREAL){
                float sub = GetReal() - op.GetReal();
                return Value(sub);
            }
            else{
                return Value();
            }
        }
        return Value();
    };
    
    // multiply this by op
    Value operator*(const Value& op)const {
       if(op.IsErr() || T == VERR){
            return Value();
        }
        if(op.IsInt()){
            if(T == VINT){
                int sub = GetInt() * op.GetInt();
                return Value(sub);
            }
            else if(T == VREAL){
                float sub = GetReal() * op.GetInt();
                return Value(sub);
            }
            else{
                return Value();
            }
            
        }

        else if(op.IsReal()){
            if(T == VINT){
                float sub = GetInt() * op.GetReal();
                return Value(sub);
            }
            else if(T == VREAL){
                float sub = GetReal() * op.GetReal();
                return Value(sub);
            }
            else{
                return Value();
            }
        }
        return Value();
    };
    
    // divide this by op
    Value operator/(const Value& op) const {
       if(op.IsErr() || T == VERR){
            return Value();
        }
        if(op.IsInt()){
            if(T == VINT){
                int sub = GetInt() / op.GetInt();
                return Value(sub);
            }
            else if(T == VREAL){
                float sub = GetReal() / op.GetInt();
                return Value(sub);
            }
            else{
                return Value();
            }
            
        }

        else if(op.IsReal()){
            if(T == VINT){
                float sub = GetInt() / op.GetReal();
                return Value(sub);
            }
            else if(T == VREAL){
                float sub = GetReal() / op.GetReal();
                return Value(sub);
            }
            else{
                return Value();
            }
        }
        return Value();
    };
    
    Value operator==(const Value& op) const{
         if(op.IsErr() || T == VERR){
            return Value();
        }
        if(op.GetType() != T){
            if(op.IsReal()){
                if(GetType() != VINT){
                    return Value();
                }
                if(op.GetReal() == GetInt()){
                    return Value(true);
                }
            }
            else if(op.GetType() == VINT){
                if(GetType() != VREAL){
                    return Value();
                }
                return(op.GetInt() == GetReal());
            }
            return Value();
           
        }
        if(op.Btemp == Btemp && op.Itemp == Itemp && op.Rtemp == Rtemp && op.Stemp == Stemp){
            return Value(true);
        }
        else{
            return Value(false);
        }
    };

	Value operator>(const Value& op) const{
       if(op.IsErr() || T == VERR){
            return Value();
        }
        bool out = false;
        if(op.IsInt()){
            if(T == VINT){
                out = GetInt() > op.GetInt();
                return Value(out);
            }
            else if(T == VREAL){
                out = GetReal() > op.GetInt();
                return Value(out);
            }
            else{
                return Value();
            }
            
        }

        else if(op.IsReal()){
            if(T == VINT){
                out = GetInt() > op.GetReal();
                return Value(out);
            }
            else if(T == VREAL){
                out = GetReal() > op.GetReal();
                return Value(out);
            }
            else{
                return Value();
            }
        }

        if(op.IsBool() && IsBool()){
            if(GetBool()){
                if(!op.GetBool()){
                    out = true;
                }
            }
            return Value(out);
        }
        return Value();

    };
    
	
	Value operator<(const Value& op) const    {
       if(op.IsErr() || T == VERR){
            return Value();
        }
        bool out = false;
        if(op.IsInt()){
            if(T == VINT){
                out = GetInt() < op.GetInt();
                return Value(out);
            }
            else if(T == VREAL){
                out = GetReal() < op.GetInt();
                return Value(out);
            }
            else{
                return Value();
            }
            
        }

        else if(op.IsReal()){
            if(T == VINT){
                out = GetInt() < op.GetReal();
                return Value(out);
            }
            else if(T == VREAL){
                out = GetReal() < op.GetReal();
                return Value(out);
            }
            else{
                return Value();
            }
        }

        if(op.IsBool() && IsBool()){
            if(op.GetBool()){
                if(!GetBool()){
                    out = true;
                }
            }
            return Value(out);
        }
        return Value();

    };

	Value operator&&(const Value& op)const{
        if(op.IsErr() || T== VERR){
            return Value();
        }
        if(op.IsBool() && IsBool()){
            bool out;
            if(op.GetBool() && GetBool()){
                out = true;
            }
            else{
                out = false;
            }
            return Value(out);

        }
        else{
            return Value();
        }
    };
	
	Value operator||(const Value& op) const{   
        if(op.IsErr() || T== VERR){
            return Value();
        }
        if(op.IsBool() && IsBool()){
            bool out;
            if(op.GetBool() || GetBool()){
                out = true;
            }
            else{
                out = false;
            }
            return Value(out);
        }
        else{
            return Value();
        }};
	
	Value operator!()const{
        if(GetType() != VBOOL){
            return Value();
        }
        if(GetBool()){
            return Value(false);
        }
        else{
            return Value(true);
        }
    };//NOT Unary operator
	
	    
    friend ostream& operator<<(ostream& out, const Value& op) {
        if( op.IsInt() ) out << op.Itemp;
        else if( op.IsString() ) out << op.Stemp;
        else if( op.IsReal()) out << fixed << showpoint << setprecision(2) << op.Rtemp;
        else if(op.IsBool()) out << (op.GetBool()? "true" : "false");
        else out << "ERROR";
        return out;
    }
};


#endif
