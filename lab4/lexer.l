
%{
    /*
    * You will need to comment this line in lab5.
    */
    #define ONLY_FOR_LEX
    
    #ifdef ONLY_FOR_LEX
    #else
    #define YYSTYPE void *
    #include "parser.h"
    #endif

    #define YY_NO_UNPUT
    #define YY_NO_INPUT
    #include <string>
    #include <iomanip>

    #ifdef ONLY_FOR_LEX
    #include <ostream>
    #include <fstream>
    #include <string.h>
    using namespace std;
    extern FILE *yyin; 
    extern FILE *yyout;
    ofstream output;
    int offset=0;

    void outputline(string s1,string s2,string s3=""){
        output.width(10);
        output<<s1;
        output.width(12);
        output<<s2;
        output.width(8);
        output<<to_string(yylineno);
        output.width(8);
        output<<s3;
        output.width(8);
        output<<offset<<endl;    
        offset+=string(yytext).size();
    }
    #endif
%}

DECIMAL ([1-9][0-9]*|0)
OCTADECIMAL ([0][1-7][0-7]*)
HEXADECIMAL (0[xX][1-9a-fA-F][0-9a-fA-F]*)
ID [[:alpha:]_][[:alpha:][:digit:]_]*
EOL (\r\n|\n|\r)
WHITE [\t ]
LINECOMMENT \/\/([^\n])*

BLOCKCOMMENTBEGIN \/\*
BLOCKCOMMENT1 \n
BLOCKCOMMENT2 .
BLOCKCOMMENTEND \*\/
%x COMMENT

%%

"int" {
    /*y
    * Questions: 
    *   Q1: Why we need to return INT in further labs?
    *   Q2: What is "INT" actually?
    */
    #ifdef ONLY_FOR_LEX
        outputline("INT","int");
    #else
        return INT;
    #endif
}
"void" {
    #ifdef ONLY_FOR_LEX
        outputline("VOID","void");
    #else
        return VOID;
    #endif 
}
"if" {
    #ifdef ONLY_FOR_LEX
        outputline("IF","if");
    #else
        return IF;
    #endif
};
"else" {
    #ifdef ONLY_FOR_LEX
        outputline("ELSE","else");
    #else
        return ELSE;
    #endif
};
"return" {
    #ifdef ONLY_FOR_LEX
        outputline("RETURN","return");
    #else
        return RETURN;
    #endif
};
"main" {
    #ifdef ONLY_FOR_LEX
        outputline("MAIN","main");
    #else
        return MAIN;
    #endif
}
"==" {
    #ifdef ONLY_FOR_LEX
        outputline("EQ","==");
    #else
        return ASSIGN;
    #endif
}
"=" {
    #ifdef ONLY_FOR_LEX
        outputline("ASSIGN","=");
    #else
        return ASSIGN;
    #endif
}
"<" {
    #ifdef ONLY_FOR_LEX
        outputline("LESS","<");
    #else
        return LESS;
    #endif
}
">" {
    #ifdef ONLY_FOR_LEX
        outputline("MORE",">");
    #else
        return MORE;
    #endif
}
"+" {
    #ifdef ONLY_FOR_LEX
        outputline("ADD","+");
    #else
        return ADD;
    #endif
}
"*" {
    #ifdef ONLY_FOR_LEX
        outputline("MULT","*");
    #else
        return MULT;
    #endif
}
"," {
    #ifdef ONLY_FOR_LEX
        outputline("COMMA",",");
    #else
        return COMMA;
    #endif
}
";" {
    #ifdef ONLY_FOR_LEX
        outputline("SEMICOLON",";");
    #else
        return SEMICOLON;
    #endif
}
"(" {
    #ifdef ONLY_FOR_LEX
        outputline("LPAREN","(");
    #else
        return LPAREN;
    #endif
}
")" {
    #ifdef ONLY_FOR_LEX
        outputline("RPAREN",")");
    #else
    return RPAREN;
    #endif
}
"{" {
    #ifdef ONLY_FOR_LEX
        outputline("LBRACE","{");
    #else
        return LBRACE;
    #endif
}
"}" {
    #ifdef ONLY_FOR_LEX
        outputline("RBRACE","}");
    #else
        return RBRACE;
    #endif
}
"." {
    #ifdef ONLY_FOR_LEX
        outputline("DOT",".");
    #else
        return DOT;
    #endif
}
"while" {
    #ifdef ONLY_FOR_LEX
        outputline("WHILE","while");
    #else
        string s(yytext);
        int siz=s.size();
        offset+=siz;
        return WHILE;
    #endif
}

{EOL} {
    yylineno++;
    offset=0;
}
{LINECOMMENT} 
{BLOCKCOMMENTBEGIN} {BEGIN COMMENT;}
<COMMENT>{BLOCKCOMMENT1} {
    yylineno++;
}       
<COMMENT>{BLOCKCOMMENT2} {}
<COMMENT>{BLOCKCOMMENTEND} {BEGIN INITIAL;}
{WHITE} {
	offset++;
}

{DECIMAL} {
    #ifdef ONLY_FOR_LEX
        string num=yytext;
        outputline("NUM",num);
    #else
        return DECIMAL;
    #endif
}
{OCTADECIMAL} {
    #ifdef ONLY_FOR_LEX
        int num = 0; 
        string s(yytext);
        int siz=s.size();
        int a=0;
        for (int i=0;i<siz;i++) {
                a= yytext[i]-'0';
                num = num*8 +a ;
            }

        outputline("NUM",std::to_string(num));
    #else
        return DECIMAL;
    #endif
}
{HEXADECIMAL} {
    #ifdef ONLY_FOR_LEX
        int num = 0; 
        string s(yytext);
        int siz=s.size();
        int a=0;
        for (int i=0;i<siz;i++) {
                if(i<2){

                }
                else{
                    if('A'<=yytext[i]&&yytext[i]<='F'){
                        a= (yytext[i]-'A')+10;
                    }
                    else{
                        a= yytext[i]-'0';
                    }
                    num = num*16 +a ; 
                }
        }
        

        outputline("NUM",std::to_string(num));
    #else
        return DECIMAL;
    #endif
}

{ID} {
    #ifdef ONLY_FOR_LEX
        string id=yytext;
        outputline("ID",id);
    #else
        return ID;
    #endif
}


%%

#ifdef ONLY_FOR_LEX
int main(int argc, char **argv){
    if(argc != 5){
        fprintf(stderr, "Argument Not Enough");
        exit(EXIT_FAILURE);
    }

    if(!(yyin = fopen(argv[1], "r"))){
        fprintf(stderr, "No such file or directory: %s", argv[1]);
        exit(EXIT_FAILURE);
    }

    output.open(argv[3]);
    output.width(10);
    output<<"token";
    output.width(12);
    output<<"lexeme";
    output.width(8);
    output<<"lineno";
    output.width(20);
    output<<"offset"<<endl;
    output.width(8);
    yylex();

    output.close();
    return 0;
}
#endif
