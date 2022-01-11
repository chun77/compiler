#ifndef __TYPE_H__
#define __TYPE_H__
#include <vector>
#include <string>
#include <SymbolTable.h>

class Type
{
private:
    int kind;
protected:
    enum {INT, VOID, FUNC, PTR};
public:
    Type(int kind) : kind(kind) {};
    virtual ~Type() {};
    virtual std::string toStr() = 0;
    bool isInt() const {return kind == INT;};
    bool isVoid() const {return kind == VOID;};
    bool isFunc() const {return kind == FUNC;};
};

class IntType : public Type
{
private:
    int size;
public:
    IntType(int size) : Type(Type::INT), size(size){};
    bool isBool() {return size==1;};
    std::string toStr();
};

class VoidType : public Type
{
public:
    VoidType() : Type(Type::VOID){};
    std::string toStr();
};

class FunctionType : public Type
{
private:
    Type *returnType;
    std::vector<Type*> paramsType;
    std::vector<SymbolEntry*>paramSe;
    bool isRet;
    bool sysy;
public:
    FunctionType(Type* returnType, std::vector<Type*> paramsType) : 
    Type(Type::FUNC), returnType(returnType), paramsType(paramsType){isRet=false; sysy=false;};
    Type* getRetType() {return returnType;};
    void setRetType(Type* type) {this->returnType=type;};
    void addParam(Type* type) {this->paramsType.push_back(type);};
    int getParamNum() {return this->paramsType.size();};
    std::vector<Type*> getParamsType() { return paramsType; };
    std::vector<SymbolEntry*> getParamSe() { return paramSe; };
    void setRet() {isRet=true;};
    bool haveRet() {return isRet;};
    void setSysy() {sysy=true;};
    bool isSysy() {return sysy;};
    std::string toStr();
};

class PointerType : public Type
{
private:
    Type *valueType;
public:
    PointerType(Type* valueType) : Type(Type::PTR) {this->valueType = valueType;};
    std::string toStr();
};

class TypeSystem
{
private:
    static IntType commonInt;
    static IntType commonBool;
    static VoidType commonVoid;
public:
    static Type *intType;
    static Type *voidType;
    static Type *boolType;
};

#endif
