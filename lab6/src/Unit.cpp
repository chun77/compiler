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
    // for (auto global : global_list)
    // {
    //     fprintf(yyout, "%s = global %s %d, align 4\n", global->toStr().c_str(),
    //                 global->getType()->toStr().c_str(),
    //                 ((IdentifierSymbolEntry*)global)->getValue());
    // }
    // for (auto &global:global_list)
    // {
    //     global->output();

    // }
    globalBB->output();
    for (auto &func : func_list)
        func->output();
}



Unit::~Unit()
{
    for(auto &func:func_list)
        delete func;
}

