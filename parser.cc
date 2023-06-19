//JOSHUA GREER CSE 340 SUMMER~

#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stack>

#include "parser.h"
#include "lexer.h"
#include "inputbuf.h"

//-------------------------------------------Global Vars------------------------------------------

std::vector<std::string> scopeSt;
int layer;
std::string vis = "public";
std::vector<Variable> vars;
std::vector<std::string> potentiallyUnecessaryErrorOutputFixing;        //totally worked and patched my error test cases btw

//-------------------------------------------Constructors---------------------------------------

Variable::Variable(Token T, std::string vis, std::string scopeInstance, int layer){
    this->ID = T.lexeme;
    this->scopeString = scopeInstance;
    this->visibility = vis;
    this->layer = layer;
}

//------------------------------------Method implementations-----------------------------------

std::string varPrint(Variable var){
    std::string temp;
    if(var.scopeString != "::"){
        temp = var.scopeString + '.' +var.ID;
    }
    else{
        temp = var.scopeString + var.ID;
    }
    return temp;
}

int varCheck(std::string lexeme, int layer){           //checks if a variable with name lexeme already exists in symbol table
    for(int i = vars.size() -1; i >= 0; i--){
        if(lexeme == vars[i].ID){
            if(vars[i].visibility == "private" && layer > vars[i].layer){
                continue;
            }
            else{
                return i;
            }
        }
    }
    return -1;
}

void symbolReaper(int deadLayer){           //called after scope ends, removes out of scope variables from table
    for(int i = vars.size()-1; i >= 0; i--){
        if(vars[i].layer == deadLayer){
            vars.pop_back();
        }
    }
}

void printinTime(){
    for(int i = 0; i < potentiallyUnecessaryErrorOutputFixing.size(); i++){
        std::cout << potentiallyUnecessaryErrorOutputFixing[i] << std::endl;
    }
}

//-------------------------------------------parsing logic below-----------------------------------

void Parser::syntax_error(){
    std::cout << "Syntax Error\n";
    exit(1);
}

void Parser::parse_program(){               //global_vars scope
    //std::cout<< "starting\n";
    
    parse_global_vars();
    parse_scope();

    printinTime();
}

void Parser::parse_global_vars(){           //epsilon | var_list SEMICOLON
    //std::cout<< "glob vars\n";
    scopeSt.push_back("::");        //global scope
    int layer = 0;

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

        Variable temp(T, vis, scopeSt[layer], layer);      //uses constructor, takes T.lexeme, and vis
        vars.push_back(temp);

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
        std::string scopeName = T.lexeme;       //hold the name for a sec
        T = lexer.GetToken();
    
        if(T.token_type == LBRACE){
            scopeSt.push_back(scopeName);       //add a scopecop
            layer +=1;

            parse_public_vars();
            parse_private_vars();
            parse_stmt_list();
    
            T = lexer.GetToken();
                if(T.token_type == RBRACE){
                    //std::cout << "works great m8 -- end scope\n";
                    scopeSt.pop_back();
                    symbolReaper(layer);
                    layer -= 1;
                }
                else if(T.token_type == ID){
                    lexer.UngetToken(T);
                    parse_stmt_list();
                }
                else{
                    //std::cout<< "point C \n";
                    //printinTime();
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
            vis = "public";
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
            vis = "private";
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
    //std::cout<< "state list -------------------------------------------------------------\n";

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
        int flag = varCheck(T.lexeme, layer);      //check to see if ID exists in variables
        int hold = -2;
        std::string holdID;
        if(flag != -1){
            hold = flag;                //if it does, return index to where
        }
        else{
            holdID = T.lexeme;          //else hold the ID name for unresolved
        }

        Token T2 = lexer.GetToken();
        if(T2.token_type == LBRACE){

            lexer.UngetToken(T2);
            lexer.UngetToken(T);
            parse_scope();
        }
        else if(T2.token_type == EQUAL){

            T = lexer.GetToken();

            if(T.token_type == ID){

                flag = varCheck(T.lexeme, layer);              //check to see if ID exists in variables
                int hold2 = -2;
                std::string holdID2;
                    if(flag != -1){
                        hold2 = flag;               //if it does, return index to where
                    }
                    else{
                        holdID2 = T.lexeme;         //else hold the ID name for unresolved
                    }

                T = lexer.GetToken();
                if(T.token_type == SEMICOLON){
                    //std::cout << "works great m8 -- end of the statement\n";
                    std::string temporary = "";
                    if(hold != -2){
                        temporary = temporary + varPrint(vars[hold]);   //if we found the index, print it
                    }
                    else{
                        temporary = temporary +  "?." + holdID;    //if we didnt, print the name with unresolved scope
                    }
                    temporary = temporary + " = ";
                    if(hold2 != -2){
                        temporary = temporary + varPrint(vars[hold2]);   //if we found the index, print it
                    }
                    else{
                        temporary = temporary + "?." + holdID2;    //if we didnt, print the name with unresolved scope
                    }
                    potentiallyUnecessaryErrorOutputFixing.push_back(temporary);
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


/*-------------------------------------comment bunker------------------------------------------

lots of comments above were used for debugging

having problems with privates
varcheck compares string to each variable in vector to see if it already exists
if it exists it updates the scope// probably an incorrect thing to do

possible - need to just create more variables in vector, then reverse varcheck to start at end and work towards global
skipping privates if they are lower layer

flipping varcheck seg faults
jk, I accessed index at .size() instead of .size()-1

removed var check before creating new variable, so now there are lots of variables and some with same names different scopes
need to update statement print updating

we changed our errors, yet fixed some

fixed removing out of scope elements from symbol table
*/