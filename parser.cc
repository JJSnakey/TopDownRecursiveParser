//JOSHUA GREER CSE 340 SUMMER~

#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "parser.h"
#include "lexer.h"
#include "inputbuf.h"


void Parser::syntax_error(){
    std::cout << "Syntax Error\n";
    exit(1);
}

void Parser::parse_program(){               //global_vars scope
    //std::cout<< "starting\n";
    
    parse_global_vars();
    parse_scope();
}

void Parser::parse_global_vars(){           //epsilon | var_list SEMICOLON
    //std::cout<< "glob vars\n";
    
    Token T = lexer.GetToken();
    
    if(T.token_type == ID){
        
        Token T2 = lexer.GetToken();
        
        if(T2.token_type == LBRACE){
            lexer.UngetToken(T2);
            lexer.UngetToken(T);        //ID LBRACE is a scope, unget t and t2 then move on
        }
        else{    
            lexer.UngetToken(T2);
            lexer.UngetToken(T);
            parse_var_list();

            Token T = lexer.GetToken();                         //after var_list we need SEMICOLON
            if(T.token_type == SEMICOLON){
                //std::cout << "works great m8 -- end global vars\n";
            }
            else{
                //std::cout<< "point A ";
                syntax_error();
            }
        }

    }

}

void Parser::parse_var_list(){              //ID | ID COMMA var_list
    //std::cout<< "var list\n";
    
    Token T = lexer.GetToken();
    if(T.token_type == ID){

        T = lexer.GetToken();
        if(T.token_type == COMMA){
            parse_var_list();
        }
        else if(T.token_type == SEMICOLON){
            lexer.UngetToken(T);
        }
        else{
            //std::cout<< "point K ";
            syntax_error();
        }
    }
    else{
        //std::cout<< "point B ";
        syntax_error();
    }
}

void Parser::parse_scope(){                 // ID LBRACE public_vars private_vars stmt_list RBRACE
    //std::cout<< "scopes bb\n";
    Token T = lexer.GetToken();
    
    if(T.token_type == ID){
        T = lexer.GetToken();
    
        if(T.token_type == LBRACE){
            parse_public_vars();
            parse_private_vars();
            parse_stmt_list();
    
            T = lexer.GetToken();
            if(T.token_type == RBRACE){
                //std::cout << "works great m8 -- end scope\n";
            }
            else{
                //std::cout<< "point C ";
                syntax_error();
            }
 
        }
 
    }
    else{
        //std::cout<< "point D";
        syntax_error();
    }
}

void Parser::parse_public_vars(){           // epsilon | PUBLIC COLON var_list SEMICOLON
    //std::cout<< "pub vars\n";
    Token T = lexer.GetToken();
    if(T.token_type == PUBLIC){
        
        T = lexer.GetToken();
        if(T.token_type == COLON){
            
            parse_var_list();

            T = lexer.GetToken();
            if(T.token_type == SEMICOLON){
                //std::cout << "works great m8 -- end pub vars\n";
            }
            else{
                //std::cout<< "point E ";
                syntax_error();
            }
        }
        else{
            //std::cout<< "point F ";
            syntax_error();
        }
    }
    else{
        lexer.UngetToken(T);
    }

}

void Parser::parse_private_vars(){          // epsilon | PRIVATE COLON var_list SEMICOLON
    //std::cout<< "priv vars\n";
    Token T = lexer.GetToken();
    if(T.token_type == PRIVATE){
        
        T = lexer.GetToken();
        if(T.token_type == COLON){
            parse_var_list();

            T = lexer.GetToken();
            if(T.token_type == SEMICOLON){
               //std::cout << "works great m8 -- end priv vars\n";
            }
            else{
                //std::cout<< "point G ";
                syntax_error();
            }
        }
        else{
            //std::cout<< "point H ";
            syntax_error();
        }
    }
    else{
        lexer.UngetToken(T);
    }
}

void Parser::parse_stmt_list(){             // stmt | stmt stmt_list
    //std::cout<< "state list \n";

    parse_stmt();
    Token T = lexer.GetToken();

    if(T.token_type == RBRACE){
        lexer.UngetToken(T);
        //std::cout << "works great m8 -- end statement list\n";
    }
    else{
        lexer.UngetToken(T);
        parse_stmt();
    }
}

void Parser::parse_stmt(){                  // ID EQUAL ID SEMICOLON | scope
    //std::cout<< "statement\n";

    Token T = lexer.GetToken();

    if(T.token_type == ID){

        Token T2 = lexer.GetToken();
        if(T2.token_type == LBRACE){

            lexer.UngetToken(T2);
            lexer.UngetToken(T);
            parse_scope();
        }
        else if(T2.token_type == EQUAL){

            T = lexer.GetToken();

            if(T.token_type == ID){

                T = lexer.GetToken();

                if(T.token_type == SEMICOLON){
                    //std::cout << "works great m8 -- end of the statement\n";
                }
                else{
                    //std::cout<< "point I ";
                    syntax_error();
                }
            }
            else{
                //std::cout<< "point J ";
                syntax_error();
            }
        }
        
    }
}

/*
comment bunker

lots of comments above were used for debugging

*/