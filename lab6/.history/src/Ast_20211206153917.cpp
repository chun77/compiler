#include "Ast.h"
#include "SymbolTable.h"
#include "Unit.h"
#include "Instruction.h"
#include "IRBuilder.h"
#include <string>
#include "Type.h"

extern FILE *yyout;
int Node::counter = 0;
IRBuilder* Node::builder = nullptr;

Node::Node()
{
    seq = counter++;
}

void Node::backPatch(std::vector<Instruction*> &list, BasicBlock*bb)
{
    for(auto &inst:list)
    {
        if(inst->isCond())
            dynamic_cast<CondBrInstruction*>(inst)->setTrueBranch(bb);
        else if(inst->isUncond())
            dynamic_cast<UncondBrInstruction*>(inst)->setBranch(bb);
    }
}

std::vector<Instruction*> Node::merge(std::vector<Instruction*> &list1, std::vector<Instruction*> &list2)
{
    std::vector<Instruction*> res(list1);
    res.insert(res.end(), list2.begin(), list2.end());
    return res;
}

void Ast::genCode(Unit *unit)
{
    IRBuilder *builder = new IRBuilder(unit);
    Node::setIRBuilder(builder);
    root->genCode();
}

void FunctionDef::genCode()
{
    Unit *unit = builder->getUnit();
    Function *func = new Function(unit, se);
    BasicBlock *entry = func->getEntry();
    // set the insert point to the entry basicblock of this function.
    builder->setInsertBB(entry);

    stmt->genCode();

    /**
     * Construct control flow graph. You need do set successors and predecessors for each basic block.
     * Todo
    */
   


}

void BinaryExpr::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Function *func = bb->getParent();
    if (op == AND)
    {
        BasicBlock *trueBB = new BasicBlock(func);  // if the result of lhs is true, jump to the trueBB.
        expr1->genCode();
        backPatch(expr1->trueList(), trueBB);
        builder->setInsertBB(trueBB);               // set the insert point to the trueBB so that intructions generated by expr2 will be inserted into it.
        expr2->genCode();
        true_list = expr2->trueList();
        false_list = merge(expr1->falseList(), expr2->falseList());
    }
    else if(op == OR)
    {
        // Todo
        // Todo
        //给子表达式2new 一个truebb
        BasicBlock *trueBB = new BasicBlock(func); // if the result of lhs is true, jump to the trueBB.
        expr1->genCode();
        //子表达式1为假时跳转到子表达式2,回填
        backPatch(expr1->falseList(), trueBB);
        builder->setInsertBB(trueBB); // set the insert point to the trueBB so that intructions generated by expr2 will be inserted into it.
        expr2->genCode();
        true_list = merge(expr1->trueList(), expr2->trueList()); //子表达式1和2为真时跳到哪还不知道,放到当前节点的true_List里,交给父节点去填
        false_list = expr2->falseList();
    }
    else if(op >= LESS && op <= GREATER)
    {
        // Todo        
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
        case LESS:
            opcode = BinaryInstruction::LESS;
            break;
        case MORE:
            opcode = BinaryInstruction::MORE;
            break;
        case LESSQ:
            opcode = BinaryInstruction::LESSQ;
            break;
        case MOREQ:
            opcode = BinaryInstruction::MOREQ;
            break;
        case EQ:
            opcode = BinaryInstruction::EQ;
            break;
        case NOTEQ:
            opcode = BinaryInstruction::NOTEQ;
            break;
        }
        new BinaryInstruction(opcode, dst, src1, src2, bb);

    }
    else if(op >= ADD && op <= SUB)
    {
        expr1->genCode();
        expr2->genCode();
        Operand *src1 = expr1->getOperand();
        Operand *src2 = expr2->getOperand();
        int opcode;
        switch (op)
        {
        case ADD:
            opcode = BinaryInstruction::ADD;
            break;
        case SUB:
            opcode = BinaryInstruction::SUB;
            break;
        case DIV:
            opcode = BinaryInstruction::DIV;
            break;
        case MUL:
            opcode = BinaryInstruction::MUL;
            break;
        case MOD:
            opcode = BinaryInstruction::MOD;
            break;
        }
        new BinaryInstruction(opcode, dst, src1, src2, bb);
    }
}

void UnaryExpr::genCode()
{

}

void Constant::genCode()
{
    // we don't need to generate code.
}

void Id::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getAddr();
    new LoadInstruction(dst, addr, bb);
}


void IfStmt::genCode()
{
    Function *func;
    BasicBlock *then_bb, *end_bb;

    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    //回填
    cond->genCode();
    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), end_bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    //设置下面要开始生成的中间代码块
    builder->setInsertBB(end_bb);
}

void IfElseStmt::genCode()
{
    
    // Todo
    Function *func;
    BasicBlock *then_bb, *else_bb, *end_bb;
    func = builder->getInsertBB()->getParent();
    then_bb = new BasicBlock(func);
    else_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    cond->genCode();
    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), else_bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(else_bb);
    elseStmt->genCode();
    else_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, else_bb);

    builder->setInsertBB(end_bb);

}

void CompoundStmt::genCode()
{
    // Todo
}

void SeqNode::genCode()
{
    // Todo
}

// void DeclStmt::genCode()
// {
//     IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(id->getSymPtr());
//     if(se->isGlobal())
//     {
//         Operand *addr;
//         SymbolEntry *addr_se;
//         addr_se = new IdentifierSymbolEntry(*se);
//         addr_se->setType(new PointerType(se->getType()));
//         addr = new Operand(addr_se);
//         se->setAddr(addr);
//     }
//     else if(se->isLocal())
//     {
//         Function *func = builder->getInsertBB()->getParent();
//         BasicBlock *entry = func->getEntry();
//         Instruction *alloca;
//         Operand *addr;
//         SymbolEntry *addr_se;
//         Type *type;
//         type = new PointerType(se->getType());
//         addr_se = new TemporarySymbolEntry(type, SymbolTable::getLabel());
//         addr = new Operand(addr_se);
//         alloca = new AllocaInstruction(addr, se);                   // allocate space for local id in function stack.
//         entry->insertFront(alloca);                                 // allocate instructions should be inserted into the begin of the entry block.
//         se->setAddr(addr);                                          // set the addr operand in symbol entry so that we can use it in subsequent code generation.
//     }
// }

void ReturnStmt::genCode()
{
    // Todo
}

void AssignStmt::genCode()
{
    BasicBlock *bb = builder->getInsertBB();
    expr->genCode();
    Operand *addr = dynamic_cast<IdentifierSymbolEntry*>(lval->getSymPtr())->getAddr();
    Operand *src = expr->getOperand();
    /***
     * We haven't implemented array yet, the lval can only be ID. So we just store the result of the `expr` to the addr of the id.
     * If you want to implement array, you have to caculate the address first and then store the result into it.
     */
    new StoreInstruction(addr, src, bb);
}

void VarDeclStmt::genCode()
{

}

void ConstDeclStmt::genCode()
{

}

void VarDecls::genCode()
{

}

void ConstDecls::genCode()
{

}

void VarDecl::genCode()
{

}

void ConstDecl::genCode()
{

}

void FuncCallExp::genCode()
{
    
}

void CallList::genCode()
{

}

void CallStmt::genCode()
{

}

void NullStmt::genCode()
{

}

void FuncParams::genCode()
{

}

void FuncParam::genCode()
{

}

void WhileStmt::genCode()
{
    Function *func;
    BasicBlock *stmt_bb, *end_bb, *cond_bb;

    func = builder->getInsertBB()->getParent();

    cond_bb = new BasicBlock(func);
    stmt_bb = new BasicBlock(func);
    end_bb = new BasicBlock(func);

    cond->genCode();
    backPatch(cond->trueList(), stmt_bb);
    backPatch(cond->falseList(), end_bb);

    builder->setInsertBB(stmt_bb);
    Stmt->genCode();
    stmt_bb = builder->getInsertBB();
    new UncondBrInstruction(cond_bb, stmt_bb);

}

void BreakStmt::genCode()
{

}

void ContinueStmt::genCode()
{

}

void Ostream::genCode()
{

}

void Istream::genCode()
{

}

void Ast::typeCheck()
{
    if(root != nullptr){
        printf("%s","begintypecheck!");
        printf("%d\n",root->getSeq());
        root->typeCheck();
    }
}

void FunctionDef::typeCheck()
{
    // Todo
    if(param!=NULL){
        param->typeCheck();
    }
    if(stmt!=NULL){
        stmt->typeCheck();
    }
    Type *t=se->getType();
    if(!dynamic_cast<IdentifierSymbolEntry*>(se)->isGlobal())
    {
        fprintf(stderr,"%s","error! function define at wrong scope\n");
    }
    // if(dynamic_cast<FunctionType*>(t)->haveRet()==false){
    //     fprintf(stderr,"%s","returnStmt loss\n");
    // }

    std::string name = dynamic_cast<IdentifierSymbolEntry*>(se)->getName();
    SymbolEntry* se1 = identifiers->lookup(name);
    if(se1 == nullptr)
    {
        printf("%s\n","function error 1");
        return ;
    }
    Type* type1 = se->getType();
    int num = dynamic_cast<FunctionType*>(type1)->getParamNum();
    Type* type2 = se1->getType();
    while(1)
    {
        int a =dynamic_cast<FunctionType*>(type2)->getParamNum();
        if(a == num)
        {
            if(se != se1)
            {
                fprintf(stderr,"%s\n","error! function overloading with wrong num of param");
                return ;
            }
            else
            break;
        }
        else{
            if(se1->next != nullptr)
                se1 = se1->next;
            else
                break;
        }
    }
}

void BinaryExpr::typeCheck()
{
    // Todo
    printf("%s%d\n","BinaryExpr typecheck",this->getSeq());
    Type *type1 = expr1->getSymPtr()->getType();
    Type *type2 = expr2->getSymPtr()->getType();
    if(type1->isVoid()){
        fprintf(stderr, "type %s in BinaryExpr is void!\n",
        type1->toStr().c_str());
        exit(EXIT_FAILURE);
    }
    if(type2->isVoid()){
        fprintf(stderr, "type %s in BinaryExpr is void!\n",
        type2->toStr().c_str());
        exit(EXIT_FAILURE);
    }
    // if(type1->toStr() != type2->toStr())
    // {
    //     fprintf(stderr, "type %s and %s mismatch\n",
    //     type1->toStr().c_str(), type2->toStr().c_str());
    //     exit(EXIT_FAILURE);
    // }
    symbolEntry->setType(type1);

}

void UnaryExpr::typeCheck()
{
    printf("%s%d\n","UnaryExpr typecheck",this->getSeq());
    expr->typeCheck();
    Type *type=expr->getSymPtr()->getType();
    if(type==TypeSystem::voidType)
    {
        this->symbolEntry->setType(TypeSystem::voidType);
    }else{
        this->symbolEntry->setType(TypeSystem::intType);
    }
}

void VarDeclStmt::typeCheck()
{
    printf("%s%d\n","varDeclstmt typecheck",this->getSeq());
    varDecls->typeCheck();
}

void ConstDeclStmt::typeCheck()
{
    printf("%s%d\n","constDeclstmt typecheck",this->getSeq());
    constDecls->typeCheck();
}

void VarDecls::typeCheck()
{
    printf("%s%d\n","varDecls typecheck",this->getSeq());
    if(varDecls!=NULL){
        varDecls->typeCheck();
    }
    if(varDecl!=NULL){
        varDecl->typeCheck();
    }
}

void ConstDecls::typeCheck()
{
    printf("%s%d\n","constDecls typecheck",this->getSeq());
    if(constDecls!=NULL){
        constDecls->typeCheck();
    }
    if(constDecl!=NULL){
        constDecl->typeCheck();
    }
}

void VarDecl::typeCheck()
{
    printf("%s%d\n","varDecl typecheck",this->getSeq());
    id->typeCheck();
    if(expr!=NULL){
        expr->typeCheck();
    }

}

void ConstDecl::typeCheck()
{
    printf("%s%d\n","constDecl typecheck",this->getSeq());
    id->typeCheck();
    if(expr!=NULL){
        expr->typeCheck();
    }else{
        printf("%s","error: const value has not been initialized!\n");
    }


}

void FuncCallExp::typeCheck()
{
    printf("%s%d\n","funcCallexp typecheck",this->getSeq());
    if(symbolEntry==NULL){
        printf("%s\n","error: function not define!");
    }
    if(callList!=NULL){
        callList->typeCheck();
    }
    Type *t=this->getSymPtr()->getType();
    if(dynamic_cast<FunctionType*>(t)->getRetType()==TypeSystem::voidType){
        this->symbolEntry->setType(TypeSystem::voidType);
    }else{
        this->symbolEntry->setType(TypeSystem::intType);
    }
}

void CallList::typeCheck()
{
    printf("%s%d\n","callList typecheck",this->getSeq());
    expr->typeCheck();

    if(callList!=NULL){
        callList->typeCheck();
    }
}

void CallStmt::typeCheck()
{
    printf("%s%d\n","CallStmt typecheck",this->getSeq());
    callExp->typeCheck();
}

void NullStmt::typeCheck()
{
    printf("%s%d\n","NullStmt typecheck",this->getSeq());
    if(expr!=NULL){
        expr->typeCheck();
    }
}

void FuncParams::typeCheck()
{
    printf("%s%d\n","funcParams typecheck",this->getSeq());
    if(funcParam!=NULL){
        funcParam->typeCheck();
    }
    if(funcParams!=NULL){
        funcParams->typeCheck();
    }
}

void FuncParam::typeCheck()
{
    printf("%s%d\n","funcParam typecheck",this->getSeq());
    if(expr!=NULL){
        expr->typeCheck();
        Type* type1=id->getSymPtr()->getType();
        Type* type2=expr->getSymPtr()->getType();
        if(!type2->isInt()){
            fprintf(stderr, "functioncall type %s and %s mismatch in line xx",
            type1->toStr().c_str(), type2->toStr().c_str());
            exit(EXIT_FAILURE);
        }
    }
    
}

void BreakStmt::typeCheck()
{

}

void ContinueStmt::typeCheck()
{

}

void WhileStmt::typeCheck()
{
    printf("%s%d\n","WhileStmt typecheck",this->getSeq());
    cond->typeCheck();
    if(Stmt!=NULL){
        Stmt->typeCheck();
    }
    SymbolEntry *se=cond->getSymPtr();
    se->setType(TypeSystem::boolType);
}

void Ostream::typeCheck()
{
    exp->typeCheck();
}

void Istream::typeCheck()
{

}

void Constant::typeCheck()
{
    // Todo
    printf("%s%d\n","Constant typecheck",this->getSeq());
    if(!this->getSymPtr()->getType()->isInt()){
        printf("%s\n","error: constant type error!");
    }
}

void Id::typeCheck()
{
    // Todo,scope ..

}

void IfStmt::typeCheck()
{
    cond->typeCheck();
    cond->getSymPtr()->setType(TypeSystem::boolType);
    // Todo
}

void IfElseStmt::typeCheck()
{
    // Todo
    cond->typeCheck();
    cond->getSymPtr()->setType(TypeSystem::boolType);
}

void CompoundStmt::typeCheck()
{
    // Todo
    printf("%s%d\n","CompoundStmt typecheck",this->getSeq());
    if(stmt!=NULL){
        stmt->typeCheck();
    }
}

void SeqNode::typeCheck()
{
    // Todo
    printf("%s%d\n","SeqNode typecheck",this->getSeq());
    stmt1->typeCheck();
    stmt2->typeCheck();
}

void ReturnStmt::typeCheck()
{
    // Todo
    printf("%s%d\n","returnStmt typecheck",this->getSeq());
    Type *type2;
    if(retValue!=NULL){
        retValue->typeCheck();
        type2=this->retValue->getSymPtr()->getType();
    }
    else{
        type2=TypeSystem::voidType;
    }
}

void AssignStmt::typeCheck()
{
    printf("%s%d\n","AssignStmt typecheck",this->getSeq());
    Type *type1 = lval->getSymPtr()->getType();
    Type *type2 = expr->getSymPtr()->getType();
    if(type1->isVoid()){
        fprintf(stderr, "type %s in BinaryExpr is void!\n",
        type1->toStr().c_str());
        exit(EXIT_FAILURE);
    }
    if(type2->isVoid()){
        fprintf(stderr, "type %s in BinaryExpr is void!\n",
        type2->toStr().c_str());
        exit(EXIT_FAILURE);
    }
    // if(type1 != type2)
    // {
    //     fprintf(stderr, "Assign:type %s and %s mismatch in line xx",
    //     type1->toStr().c_str(), type2->toStr().c_str());
    //     exit(EXIT_FAILURE);
    // }
}

void BinaryExpr::output(int level)
{
    std::string op_str;
    switch(op)
    {
        case ADD:
            op_str = "add";
            break;
        case SUB:
            op_str = "sub";
            break;
        case AND:
            op_str = "and";
            break;
        case OR:
            op_str = "or";
            break;
        case LESS:
            op_str = "less";
            break;
        case MORE:
            op_str = "more";
            break;
        case MUL:
            op_str = "mul";
            break;
        case DIV:
            op_str = "div";
            break;
        case MOD:
            op_str = "mod";
            break;
        case EQ:
            op_str = "eq";
            break;    
        case NOTEQ:
            op_str = "noteq";
            break;
        case MOREQ:
            op_str = "moreq";
            break;
        case LESSQ:
            op_str = "lessq";
            break;        
    }
    fprintf(yyout, "%*cBinaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr1->output(level + 4);
    expr2->output(level + 4);
}

void UnaryExpr::output(int level)
{
    std::string op_str;
    switch(op)
    {
        case ADD:
            op_str = "pos";
            break;
        case SUB:
            op_str = "nag";
            break;
        case NOT:
            op_str = "not";
            break;
    }
    fprintf(yyout, "%*cUnaryExpr\top: %s\n", level, ' ', op_str.c_str());
    expr->output(level + 4);
}

void Ast::output()
{
    fprintf(yyout, "program\n");
    if(root != nullptr)
        root->output(4);
}

void Constant::output(int level)
{
    std::string type, value;
    type = symbolEntry->getType()->toStr();
    value = symbolEntry->toStr();
    fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ',
            value.c_str(), type.c_str());
}

void Id::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry*>(symbolEntry)->getScope();
    fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ',
            name.c_str(), scope, type.c_str());
}

void CompoundStmt::output(int level)
{
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    stmt->output(level + 4);
}

void SeqNode::output(int level)
{
    fprintf(yyout, "%*cSequence\n", level, ' ');
    stmt1->output(level + 4);
    stmt2->output(level + 4);
}

void VarDeclStmt::output(int level)
{
    fprintf(yyout, "%*cVarDeclStmt\n", level, ' ');
    varDecls->output(level + 4);
}

void ConstDeclStmt::output(int level)
{
    fprintf(yyout, "%*cConstDeclStmt\n", level, ' ');
    constDecls->output(level + 4);
}

void VarDecls::output(int level)
{
    fprintf(yyout, "%*cVarDecls\n", level, ' ');
    varDecl->output(level + 4);
    varDecls->output(level + 4);
}

void ConstDecls::output(int level)
{
    fprintf(yyout, "%*cConstDecls\n", level, ' ');
    constDecl->output(level + 4);
    constDecls->output(level + 4);
}

void VarDecl::output(int level)
{
    fprintf(yyout, "%*cVarDecl\n", level, ' ');
    id->output(level + 4);
    if(expr!=NULL){
        expr->output(level + 4);
    }
}

void ConstDecl::output(int level)
{
    fprintf(yyout, "%*cConstDecl\n", level, ' ');
    id->output(level + 4);
    if(expr!=NULL){
        expr->output(level + 4);
    }
}

void FuncParams::output(int level)
{
    fprintf(yyout, "%*cFuncParams\n", level, ' ');
    if(funcParam!=NULL){
        funcParam->output(level + 4);
    }
    if(funcParams!=NULL){
        funcParams->output(level + 4);
    }
}

void FuncParam::output(int level)
{
    fprintf(yyout, "%*cFuncParam\n", level, ' ');
    if(id!=NULL){
    id->output(level + 4);
    }
    if(expr!=NULL){
        expr->output(level + 4);
    }
}

void CallList::output(int level)
{
    fprintf(yyout, "%*cFuncCallList\n", level, ' ');
    expr->output(level+4);
    if(callList!=NULL){
        callList->output(level+4);
    }
}

void FuncCallExp::output(int level)
{
    fprintf(yyout, "%*cFuncCallExp\n", level, ' ');
    if(callList!=NULL){
        callList->output(level+4);
    }
}

void CallStmt::output(int level)
{
    fprintf(yyout, "%*cFuncCallStmt\n", level, ' ');
    callExp->output(level+4);
}

void NullStmt::output(int level)
{
    fprintf(yyout, "%*cNullStmt\n", level, ' ');
    if(expr!=NULL){
        expr->output(level+4);
    }
}

void IfStmt::output(int level)
{
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    cond->output(level + 4);
    if(thenStmt!=NULL){
        thenStmt->output(level + 4);
    }
}

void IfElseStmt::output(int level)
{
    fprintf(yyout, "%*cIfElseStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
    elseStmt->output(level + 4);
}
void WhileStmt::output(int level)
{
    fprintf(yyout, "%*cWhileStmt\n", level, ' ');
    cond->output(level + 4);
    Stmt->output(level + 4);
}
void ReturnStmt::output(int level)
{
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    retValue->output(level + 4);
}

void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}
void BreakStmt::output(int level)
{
    fprintf(yyout, "%*cBreakStmt\n", level, ' ');
}

void ContinueStmt::output(int level)
{
    fprintf(yyout, "%*cContinueStmt\n", level, ' ');
}
void FunctionDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine function name: %s, type: %s\n", level, ' ', 
            name.c_str(), type.c_str());
    if(param!=NULL){
        param->output(level + 4);
    }
    stmt->output(level + 4);
    
}

void Ostream::output(int level)
{
    fprintf(yyout, "%*cOstream\n", level, ' ');
    exp->output(level + 4);
}
void Istream::output(int level)
{
    fprintf(yyout, "%*cIstream\n", level, ' ');
}
