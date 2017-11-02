
#include"lexical_analyzer.c" 
void global_declaration()    //识别全局变量 最前置的词法解析（不支持#)  
{
       // global_declaration ::= enum_decl | variable_decl | function_decl
    //
    // enum_decl ::= 'enum' [id] '{' id ['=' 'num'] {',' id ['=' 'num'} '}'
    //
    // variable_decl ::= type {'*'} id { ',' {'*'} id } ';'
    //
// function_decl ::= type {'*'} id '(' parameter_decl ')' '{' body_decl '}'
}