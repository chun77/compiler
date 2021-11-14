#include "Ast.h"
#include "SymbolTable.h"
#include <string>
#include "Type.h"

extern FILE *yyout;
int Node::counter = 0;

Node::Node()
{
    seq = counter++;
}

void Ast::output()
{
    fprintf(yyout, "program\n");
    if(root != nullptr)
        root->output(4);
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
    funcParam->output(level + 4);
    funcParams->output(level + 4);
}

void FuncParam::output(int level)
{
    fprintf(yyout, "%*cFuncParam\n", level, ' ');
    id->output(level + 4);
    if(expr!=NULL){
        expr->output(level + 4);
    }
}

void CallList::output(int level)
{
    fprintf(yyout, "%*cFuncCallList\n", level, ' ');
    call->output(level+4);
    if(callList!=NULL){
        callList->output(level+4);
    }
}


void FuncCallStmt::output(int level)
{
    fprintf(yyout, "%*cFuncCallStmt\n", level, ' ');
    callList->output(level+4);
}

void IfStmt::output(int level)
{
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
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

void BreakStmt::output(int level)
{
    fprintf(yyout, "%*cBreakStmt\n", level, ' ');
}

void ContinueStmt::output(int level)
{
    fprintf(yyout, "%*cContinueStmt\n", level, ' ');
}

void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}

void FunctionDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine function name: %s, type: %s\n", level, ' ', 
            name.c_str(), type.c_str());
    stmt->output(level + 4);
}
