#ifndef __UNIT_H__
#define __UNIT_H__

#include <vector>
#include "Function.h"
#include "AsmBuilder.h"

class Unit
{
    typedef std::vector<Function *>::iterator iterator;
    typedef std::vector<Function *>::reverse_iterator reverse_iterator;
    typedef std::vector<Instruction *>::iterator giterator;
private:
    std::vector<Function *> func_list;
    BasicBlock* globalBB;
    std::vector<SymbolEntry*> sysy_list;
public:
    Unit(){globalBB=new BasicBlock;};
    ~Unit() ;
    void insertFunc(Function *);
    void removeFunc(Function *);
    void insertDecl(SymbolEntry *);
    void output() const;
    BasicBlock* getGlobalBB(){return globalBB;};
    iterator begin() { return func_list.begin(); };
    iterator end() { return func_list.end(); };
    reverse_iterator rbegin() { return func_list.rbegin(); };
    reverse_iterator rend() { return func_list.rend(); };
    void genMachineCode(MachineUnit* munit);
};

#endif
