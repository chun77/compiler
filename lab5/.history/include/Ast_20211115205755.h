#ifndef __AST_H__
#define __AST_H__

#include <fstream>

class SymbolEntry;

class Node
{
private:
    static int counter;
    int seq;
public:
    Node();
    int getSeq() const {return seq;};
    virtual void output(int level) = 0;
};

class ExprNode : public Node
{
protected:
    SymbolEntry *symbolEntry;
public:
    ExprNode(SymbolEntry *symbolEntry) : symbolEntry(symbolEntry){};
};


class FuncCallExp : public ExprNode
{
private:
    ExprNode* callList;
public:
    FuncCallExp(SymbolEntry* se, ExprNode* callList): ExprNode(se), callList(callList) {};
    void output(int level);
};

class BinaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr1, *expr2;
public:
    enum {ADD, SUB, AND, OR, LESS, MORE, DIV, MUL,LESSQ,MOREQ,EQ, NOTEQ, MOD};
    BinaryExpr(SymbolEntry *se, int op, ExprNode*expr1, ExprNode*expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2){};
    void output(int level);
};

class UnaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr;
public:
    enum {ADD,SUB,NOT};
    UnaryExpr(SymbolEntry *se,int op, ExprNode* expr) : ExprNode(se),op(op),expr(expr) {};
    void output(int level);
};

class Constant : public ExprNode
{
public:
    Constant(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
};

class Id : public ExprNode
{
public:
    Id(SymbolEntry *se) : ExprNode(se){};
    void output(int level);
};

class StmtNode : public Node
{};

class CompoundStmt : public StmtNode
{
private:
    StmtNode *stmt;
public:
    CompoundStmt(StmtNode *stmt) : stmt(stmt) {};
    void output(int level);
};

class SeqNode : public StmtNode
{
private:
    StmtNode *stmt1, *stmt2;
public:
    SeqNode(StmtNode *stmt1, StmtNode *stmt2) : stmt1(stmt1), stmt2(stmt2){};
    void output(int level);
};

class VarDeclStmt : public StmtNode
{
private:
    StmtNode *varDecls;
public:
    VarDeclStmt(StmtNode *varDecls): varDecls(varDecls){};
    void output(int level);   
};

class ConstDeclStmt : public StmtNode
{
private:
    StmtNode *constDecls;
public:
    ConstDeclStmt(StmtNode *constDecls): constDecls(constDecls){};
    void output(int level);   
};

class VarDecls : public StmtNode
{
private:
    StmtNode *varDecl,*varDecls;
public:
    VarDecls(StmtNode *varDecl,StmtNode *varDecls): varDecl(varDecl), varDecls(varDecls){};
    void output(int level);
};

class ConstDecls : public StmtNode
{
private:
    StmtNode *constDecl,*constDecls;
public:
    ConstDecls(StmtNode *constDecl,StmtNode *constDecls): constDecl(constDecl), constDecls(constDecls){};
    void output(int level);
};

class VarDecl : public StmtNode
{
private:
    Id *id;
    ExprNode *expr;
public:
    VarDecl(Id *id,ExprNode *expr) : id(id),expr(expr){};
    void output(int level);
};

class ConstDecl : public StmtNode
{
private:
    Id *id;
    ExprNode *expr;
public:
    ConstDecl(Id *id, ExprNode* expr) : id(id), expr(expr){};
    void output(int level);
};

class CallList : public ExprNode
{
private:
    ExprNode* expr;
    ExprNode* callList;
public:
    CallList(SymbolEntry*se, ExprNode* expr, ExprNode* callList) : ExprNode(se),expr(expr), callList(callList) {};
    void output(int level);
};

class CallStmt : public StmtNode
{
private:
    ExprNode* callExp;
public:
    CallStmt(ExprNode* callExp) : callExp(callExp) {};
    void output(int level);
};

class NullStmt : public StmtNode
{
private:
    ExprNode* expr;
public:
    NullStmt(ExprNode* expr): expr(expr) {};
    void output(int level);
};

class FuncParams : public StmtNode
{
private:
    StmtNode* funcParam,*funcParams;
public:
    FuncParams(StmtNode* funcParam, StmtNode* funcParams) : funcParam(funcParam), funcParams(funcParams) {};
    void output(int level);
};

class FuncParam : public StmtNode
{
private:
    Id* id;
    ExprNode* expr;
public:
    FuncParam(Id* id,ExprNode* expr) : id(id), expr(expr) {};
    void output(int level);
};


class IfStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
public:
    IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt){};
    void output(int level);
};

class IfElseStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
    StmtNode *elseStmt;
public:
    IfElseStmt(ExprNode *cond, StmtNode *thenStmt, StmtNode *elseStmt) : cond(cond), thenStmt(thenStmt), elseStmt(elseStmt) {};
    void output(int level);
};

class ReturnStmt : public StmtNode
{
private:
    ExprNode *retValue;
public:
    ReturnStmt(ExprNode*retValue) : retValue(retValue) {};
    ReturnStmt() : retValue(nullptr) {};
    void output(int level);
};


class BreakStmt : public StmtNode
{
public:
    BreakStmt(){};
    void output(int level);
};

class ContinueStmt : public StmtNode
{
public:
    ContinueStmt(){};
    void output(int level);
};

class WhileStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *Stmt;
public:
    WhileStmt(ExprNode *cond, StmtNode *Stmt) : cond(cond), Stmt(Stmt){};
    void output(int level);
};

class AssignStmt : public StmtNode
{
private:
    ExprNode *lval;
    ExprNode *expr;
public:
    AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr) {};
    void output(int level);
};

class FunctionDef : public StmtNode
{
private:
    SymbolEntry *se;
    StmtNode *stmt;
    StmtNode *param;
public:
    FunctionDef(SymbolEntry *se,StmtNode *stmt,StmtNode *param) : se(se), stmt(stmt), param(param){ };
    void output(int level);
};

class Ostream : public ExprNode
{
private:
    ExprNode * exp;
public:
    Ostream(SymbolEntry* se,ExprNode * exp):ExprNode(se),exp(exp){};
    void output(int level);
};

class Istream : public ExprNode
{
private:

public:
    Istream(SymbolEntry* se): ExprNode(se){};
    void output(int level);
};

class Ast
{
private:
    Node* root;
public:
    Ast() {root = nullptr;}
    void setRoot(Node*n) {root = n;}
    void output();
};

#endif
