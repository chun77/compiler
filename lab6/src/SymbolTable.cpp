#include "SymbolTable.h"
#include "Type.h"
#include <iostream>
#include <sstream>
using namespace std;

SymbolEntry::SymbolEntry(Type *type, int kind) 
{
    this->type = type;
    this->kind = kind;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type, int value) : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    this->value = value;
}

std::string ConstantSymbolEntry::toStr()
{
    std::ostringstream buffer;
    buffer << value;
    return buffer.str();
}

IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type, std::string name, int scope) : SymbolEntry(type, SymbolEntry::VARIABLE), name(name)
{
    this->scope = scope;
    addr = nullptr;
}

std::string IdentifierSymbolEntry::toStr()
{
    return "@" + name;
}

TemporarySymbolEntry::TemporarySymbolEntry(Type *type, int label) : SymbolEntry(type, SymbolEntry::TEMPORARY)
{
    this->label = label;
}

std::string TemporarySymbolEntry::toStr()
{
    std::ostringstream buffer;
    buffer << "%t" << label;
    return buffer.str();
}

SymbolTable::SymbolTable()
{
    prev = nullptr;
    level = 0;

    Type* funcType1 = new FunctionType(TypeSystem::intType,{});
    dynamic_cast<FunctionType*>(funcType1)->setRetType(TypeSystem::intType);
    dynamic_cast<FunctionType*>(funcType1)->setSysy();
    SymbolEntry *se1 = new IdentifierSymbolEntry(funcType1, "getint", 0);
    this->install("getint",se1);
    vector<Type*> vec;
    vec.push_back(TypeSystem::intType);
    Type* funcType2= new FunctionType(TypeSystem::voidType,vec);
    dynamic_cast<FunctionType*>(funcType2)->setRetType(TypeSystem::voidType);
    dynamic_cast<FunctionType*>(funcType2)->setSysy();
    SymbolEntry *se2 = new IdentifierSymbolEntry(funcType2, "putint", 0);
    this->install("putint",se2);
    SymbolEntry *se3 = new IdentifierSymbolEntry(funcType2, "putch", 0);
    this->install("putch",se3);

}

SymbolTable::SymbolTable(SymbolTable *prev)
{
    this->prev = prev;
    this->level = prev->level + 1;
}

/*
    Description: lookup the symbol entry of an identifier in the symbol table
    Parameters: 
        name: identifier name
    Return: pointer to the symbol entry of the identifier

    hint:
    1. The symbol table is a stack. The top of the stack contains symbol entries in the current scope.
    2. Search the entry in the current symbol table at first.
    3. If it's not in the current table, search it in previous ones(along the 'prev' link).
    4. If you find the entry, return it.
    5. If you can't find it in all symbol tables, return nullptr.
*/
SymbolEntry* SymbolTable::lookup(std::string name)
{
    // Todo
    // std::map<std::string, SymbolEntry*>::iterator it;
    // std::map<std::string, SymbolEntry*> prevSymbolTable;
    // SymbolTable*p=this;
    // it = symbolTable.find(name);
    // if(it!=symbolTable.end())
    // {
    //     return symbolTable[name];
    // }
    // else{
    //     while(p->prev!=nullptr)
    //     {
    //         prevSymbolTable=p->prev->symbolTable;
    //         it=prevSymbolTable.find(name);
    //         if(it!=prevSymbolTable.end())
    //         {
    //             return prevSymbolTable[name];
    //         }
    //         p=p->prev;
    //     }
    // }
    // return nullptr;
    map<string,SymbolEntry*>::iterator it;
    SymbolTable*p=this;
    it=p->symbolTable.find(name);
    while(it==p->symbolTable.end()&&p->level!=0)
    {
        p=p->prev;
        it=p->symbolTable.find(name);
    }
    if(it!=p->symbolTable.end())
    {
        return it->second;
    }
    else
        return nullptr;
}

// install the entry into current symbol table.
void SymbolTable::install(std::string name, SymbolEntry* entry)
{
    symbolTable[name] = entry;

}

int SymbolTable::counter = 0;
static SymbolTable t;
SymbolTable *identifiers = &t;
SymbolTable *globals = &t;
SymbolEntry *current = NULL;
