#include "Unit.h"
#include "SymbolTable.h"
#include "Type.h"
extern FILE* yyout;
void Unit::insertFunc(Function *f)
{
    func_list.push_back(f);
}

void Unit::removeFunc(Function *func)
{
    func_list.erase(std::find(func_list.begin(), func_list.end(), func));
}

void Unit::output() const
{
    globalBB->output();
    for (auto &func : func_list)
        func->output();
    for (auto it=sysy_list.begin();it!=sysy_list.end();it++){
        FunctionType* type=dynamic_cast<FunctionType*>((*it)->getType());
        string str=type->toStr();
        string name=dynamic_cast<IdentifierSymbolEntry*>(*it)->getName();
        fprintf(yyout,"declare %s @%s(",type->getRetType()->toStr().c_str(),name.c_str());
        if(type->getParamNum()!=0){
            fprintf(yyout,"i32");
        }
        fprintf(yyout,")\n");
    }
}

void Unit::insertDecl(SymbolEntry* se){
    auto it = find(sysy_list.begin(),sysy_list.end(),se);
    if(it==sysy_list.end()){
        sysy_list.push_back(se);
    }
}

void Unit::genMachineCode(MachineUnit* munit) 
{
    AsmBuilder* builder = new AsmBuilder();
    builder->setUnit(munit);
    globalBB->genMachineCode(builder);
    for (auto &func : func_list)
        func->genMachineCode(builder);
}

Unit::~Unit()
{
    for(auto &func:func_list)
        delete func;
}


