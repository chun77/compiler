%code top{
    #include <iostream>
    #include <assert.h>
    #include "parser.h"
    extern Ast ast;
    int yylex();
    int yyerror( char const * );
}

%code requires {
    #include "Ast.h"
    #include "SymbolTable.h"
    #include "Type.h"
}

%union {
    int itype;
    char* strtype;
    StmtNode* stmttype;
    ExprNode* exprtype;
    Type* type;
}

%start Program
%token <strtype> ID 
%token <itype> INTEGER
%token IF ELSE
%token WHILE FOR BREAK CONTINUE
%token INT VOID CONST
%token LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA LBRACKET RBRACKET GETINT PUTINT PUTCH
%token ASSIGN
%token LESS EQ MORE LESSQ MOREQ NOTEQ
%token MUL DIV MOD
%token OR AND NOT
%token ADD SUB 
%token RETURN


%nterm <stmttype> Stmts Stmt AssignStmt BlockStmt IfStmt WhileStmt ReturnStmt BreakStmt ContinueStmt DeclStmt ConstDeclStmt ConstDecls ConstDecl VarDeclStmt VarDecls VarDecl FuncDef FuncParams FuncParam NullStmt
%nterm <exprtype> Exp AddExp MulExp Cond LOrExp PrimaryExp LVal RelExp LAndExp UnaryExp FuncCallExp CallList Istream Ostream 
%nterm <type> Type

%precedence THEN
%precedence ELSE
%%
Program
    : Stmts {
        ast.setRoot($1);
    }
    ;
Stmts
    : Stmt {$$=$1;}
    | Stmts Stmt{
        $$ = new SeqNode($1, $2);
    }
    ;
Stmt
    : AssignStmt {$$=$1;}
    | BlockStmt {$$=$1;}
    | IfStmt {$$=$1;}
    | WhileStmt {$$=$1;}
    | ReturnStmt {$$=$1;}
    | BreakStmt {$$=$1;}
    | ContinueStmt {$$=$1;}
    | DeclStmt {$$=$1;}
    | FuncDef {$$=$1;}

    | NullStmt {$$=$1;}
    ;
LVal
    : ID {
        SymbolEntry *se;
        se = identifiers->lookup($1);
        if(se == nullptr)
        {
            fprintf(stderr, "identifier \"%s\" is undefined\n", (char*)$1);
            delete [](char*)$1;
            assert(se != nullptr);
        }
        $$ = new Id(se);
        delete []$1;
    }
    ;

AssignStmt
    :
    LVal ASSIGN Exp SEMICOLON{
        $$ = new AssignStmt($1, $3);
    }
    ;
BlockStmt
    :   LBRACE 
        {identifiers = new SymbolTable(identifiers);} 
        Stmts RBRACE 
        {
            $$ = new CompoundStmt($3);
            SymbolTable *top=identifiers;
            identifiers = identifiers->getPrev();
            delete top;
        }
    ;
IfStmt
    : IF LPAREN Cond RPAREN Stmt %prec THEN {
        $$ = new IfStmt($3, $5);
    }
    | IF LPAREN Cond RPAREN LBRACE RBRACE {
        $$ = new IfStmt($3,nullptr);
    }
    | IF LPAREN Cond RPAREN Stmt ELSE Stmt {
        $$ = new IfElseStmt($3, $5, $7);
    }

    ;
WhileStmt
    : WHILE LPAREN Cond RPAREN Stmt {
        $$ = new WhileStmt($3, $5);
    } 
    ;
ReturnStmt
    :
    RETURN Exp SEMICOLON{
        $$ = new ReturnStmt($2);
    }
    |
    RETURN SEMICOLON{
        $$ = new ReturnStmt(nullptr);
    }
    ;
BreakStmt
    :
    BREAK SEMICOLON{
        $$ = new BreakStmt();
    }
ContinueStmt
    :
    CONTINUE SEMICOLON{
        $$ = new ContinueStmt();
    }
    ;
NullStmt
    :
    SEMICOLON 
    {
        $$=new NullStmt(nullptr);
    }
    |
    Exp SEMICOLON
    {
        $$=new NullStmt($1);
    }
    ;
Exp
    :
    AddExp {$$ = $1;}
    ;
Cond
    :
    LOrExp {$$ = $1;}
    ;    

AddExp
    :
    MulExp {$$ = $1;}
    |
    AddExp ADD MulExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::ADD, $1, $3);
    }
    |
    AddExp SUB MulExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::SUB, $1, $3);
    }
    ;

MulExp
    :
    UnaryExp {$$ = $1;}
    |
    MulExp MUL UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MUL, $1, $3);
    }
    |
    MulExp DIV UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::DIV, $1, $3);
    }
    |
    MulExp MOD UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MOD, $1, $3);
    }
    ;

UnaryExp
    :
    PrimaryExp {$$=$1;}
    |
    ADD UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::ADD, $2);
    }
    |
    SUB UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::SUB, $2);
    }
    |
    NOT UnaryExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new UnaryExpr(se, UnaryExpr::NOT, $2);
    }
    ;


PrimaryExp
    :
    LVal {
        $$ = $1;
    }  
    |  
    LPAREN Exp RPAREN {
        $$=$2;
    }
    | 
    INTEGER {
        SymbolEntry *se = new ConstantSymbolEntry(TypeSystem::intType, $1);
        $$ = new Constant(se);
    }
    |
    FuncCallExp{
        $$=$1;
    }
    ;

RelExp
    :
    AddExp {$$ = $1;}
    |
    RelExp LESS AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESS, $1, $3);
    }
    |
    RelExp MORE AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MORE, $1, $3);
    }
    |
    RelExp LESSQ AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::LESSQ, $1, $3);
    }
    |
    RelExp MOREQ AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::MOREQ, $1, $3);
    }
    |
    RelExp EQ AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::EQ, $1, $3);
    }
    |
    RelExp NOTEQ AddExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::NOTEQ, $1, $3);
    }
    ;
LAndExp
    :
    RelExp {$$ = $1;}
    |
    LAndExp AND RelExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::AND, $1, $3);
    }
    ;
LOrExp
    :
    LAndExp {$$ = $1;}
    |
    LOrExp OR LAndExp
    {
        SymbolEntry *se = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
        $$ = new BinaryExpr(se, BinaryExpr::OR, $1, $3);
    }
    ;
Type
    : INT {
        $$ = TypeSystem::intType;
    }
    | VOID {
        $$ = TypeSystem::voidType;
    }
    ;
DeclStmt
    : VarDeclStmt {$$=$1;}
    |
    ConstDeclStmt {$$=$1;}
    ;
VarDeclStmt
    :Type VarDecls SEMICOLON {
        $$=$2;
    };
ConstDeclStmt
    :CONST Type ConstDecls SEMICOLON {
        $$=$3;
    };
VarDecls
    : VarDecl {$$=$1;}
    | 
    VarDecl COMMA VarDecls {
        $$ = new VarDecls($1, $3);
    };
ConstDecls
    : ConstDecl {$$=$1;}
    |
    ConstDecl COMMA ConstDecls {
        $$ = new ConstDecls($1, $3);
    }

VarDecl
    :
    ID {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new VarDecl(new Id(se),nullptr);
        delete []$1;
    }
    |
    ID ASSIGN Exp{
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new VarDecl(new Id(se),$3);
        delete []$1;
    }
    ;
ConstDecl
    :
    ID {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new ConstDecl(new Id(se),nullptr);
        delete []$1;
    }
    |
    ID ASSIGN Exp{
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$ = new ConstDecl(new Id(se),$3);
        delete []$1;
    }
    ;

FuncDef
    :
    Type ID LPAREN FuncParams RPAREN BlockStmt
    {
        Type *funcType;
        funcType = new FunctionType($1,{});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);

        $$ = new FunctionDef(se, $4, $6);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    |
    Type ID LPAREN RPAREN BlockStmt
    {
        Type *funcType;
        funcType = new FunctionType($1,{});
        SymbolEntry *se = new IdentifierSymbolEntry(funcType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        identifiers = new SymbolTable(identifiers);

        $$ = new FunctionDef(se, nullptr, $5);
        SymbolTable *top = identifiers;
        identifiers = identifiers->getPrev();
        delete top;
        delete []$2;
    }
    ;

FuncParams 
    :
    FuncParam {$$=$1;}
    |
    FuncParam COMMA FuncParams {
        $$=new FuncParams($1,$3);
    }
    ;

FuncParam
    :
    Type ID {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        $$ = new FuncParam(new Id(se),nullptr);
        delete []$2;
    }
    |
    Type ID ASSIGN Exp{
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $2, identifiers->getLevel());
        identifiers->install($2, se);
        $$ = new FuncParam(new Id(se),$4);
        delete []$2;
    }
    ;
Istream
    :GETINT LPAREN RPAREN
    {
        $$=new Istream(nullptr);
    }
Ostream
    :PUTINT LPAREN Exp RPAREN
    {
        $$=new Ostream(nullptr,$3);
    }
    |
    PUTCH LPAREN Exp RPAREN
    {
        $$=new Ostream(nullptr,$3);
    }
CallList
    :
    Exp {$$=$1;}
    |
    Exp COMMA CallList
    {
        $$=new CallList(nullptr,$1,$3);
    }
    ;

FuncCallExp
    :
    ID LPAREN RPAREN
    {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$=new FuncCallExp(se,nullptr);
        delete []$1;
    }
    |
    ID LPAREN CallList RPAREN
    {
        SymbolEntry *se;
        se = new IdentifierSymbolEntry(TypeSystem::intType, $1, identifiers->getLevel());
        identifiers->install($1, se);
        $$=new FuncCallExp(se,$3);
        delete[] $1;
    }
    |
    Istream{$$ = $1;}
    |
    Ostream{$$ = $1;}
    ;

%%

int yyerror(char const* message)
{
    std::cerr<<message<<std::endl;
    return -1;
}
