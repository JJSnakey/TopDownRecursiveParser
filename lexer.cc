/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 * 
 * JOSHUA GREER CSE 340 SUMMER~
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"
#include "parser.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "PUBLIC", "PRIVATE",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRACE", "RBRACE", "ID", "ERROR"
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}


bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}


Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}


TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '{':
            tmp.token_type = LBRACE;
            return tmp;
        case '}':
            tmp.token_type = RBRACE;
            return tmp;
        case '/':                           //if /
            input.GetChar(c);               // check for other /
            if(c == '/'){                   // if // then its a comment
                while(c != '\n'){           // cycle letters till new line or end of input
                    input.GetChar(c);
                    if(input.EndOfInput()){
                        tmp.token_type = END_OF_FILE;
                        return tmp;
                    }
                }
                return LexicalAnalyzer::GetToken();     //call the funct
            }
            else{
                tmp.token_type = ERROR;
                std::cout << "comment slashy protocol\n"; 
                return tmp;
            }
        case 'p':               //an over-complicated public / private keyword check
            input.GetChar(c);   //u for pub, r for priv

            if(c == 'u'){
                input.GetChar(c);
                if(c == 'b'){
                   input.GetChar(c); 
                   if(c == 'l'){
                        input.GetChar(c);
                        if(c == 'i'){
                            input.GetChar(c);
                            if(c == 'c'){
                                tmp.token_type = PUBLIC;
                                return tmp;
                            }
                            else{
                                input.UngetChar(c);
                                input.UngetChar('i');
                                input.UngetChar('l');
                                input.UngetChar('b');
                                input.UngetChar('u');
                                input.UngetChar('p');
                            }
                        }
                        else{
                            input.UngetChar(c);
                            input.UngetChar('l');
                            input.UngetChar('b');
                            input.UngetChar('u');
                            input.UngetChar('p');
                        }
                   }
                   else{
                        input.UngetChar(c);
                        input.UngetChar('b');
                        input.UngetChar('u');
                        input.UngetChar('p');
                    }
                }
                else{
                    input.UngetChar(c);
                    input.UngetChar('u');
                    input.UngetChar('p');
                }
            }
            else if (c == 'r'){

                input.GetChar(c);
                    if(c == 'i'){
                        input.GetChar(c); 
                        if(c == 'v'){
                            input.GetChar(c);
                            if(c == 'a'){
                                input.GetChar(c);
                                if(c == 't'){
                                    input.GetChar(c);
                                    if(c == 'e'){
                                        tmp.token_type = PRIVATE;
                                        return tmp;
                                    }
                                    else{
                                    input.UngetChar(c);
                                    input.UngetChar('t');
                                    input.UngetChar('a');
                                    input.UngetChar('v');
                                    input.UngetChar('i');
                                    input.UngetChar('r');
                                    input.UngetChar('p');
                                }
                                }
                                else{
                                    input.UngetChar(c);
                                    input.UngetChar('a');
                                    input.UngetChar('v');
                                    input.UngetChar('i');
                                    input.UngetChar('r');
                                    input.UngetChar('p');
                                }
                            }
                            else{
                                input.UngetChar(c);
                                input.UngetChar('v');
                                input.UngetChar('i');
                                input.UngetChar('r');
                                input.UngetChar('p');
                            }
                    }
                    else{
                            input.UngetChar(c);
                            input.UngetChar('i');
                            input.UngetChar('r');
                            input.UngetChar('p');
                        }
                    }
                    else{
                        input.UngetChar(c);
                        input.UngetChar('r');
                        input.UngetChar('p');
                    }
            }
            else{
                input.UngetChar(c);
                input.UngetChar('p');
            }
            
        default:
            if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

int main()
{
    Parser parser;

    parser.parse_program();

}


/*
comment bunker

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input


// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//

/*------------------------------------discarded
bool LexicalAnalyzer::SkipComment()
{
    char c;
    char d;
    bool comment_encountered = false;

    input.GetChar(c);
    input.GetChar(d);

    if(c == '/' && d == '/'){

        comment_encountered = true;    
        while (!input.EndOfInput() && c != '\n') {
            input.GetChar(c);
        }
        line_no += (c == '\n');
    }
    else{
        input.UngetChar(d);
        input.UngetChar(c);
    }
    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return comment_encountered;
}
*/