//JOSHUA GREER CSE 340 SUMMER~

#ifndef __PARSER__H__
#define __PARSER__H__

#include "lexer.h"
#include "inputbuf.h"


class Parser{
    public:
//function prototypes
        void syntax_error();
        void parse_program();
        void parse_global_vars();
        void parse_var_list();
        void parse_scope();
        void parse_public_vars();
        void parse_private_vars();
        void parse_stmt_list();
        void parse_stmt();
    
        LexicalAnalyzer lexer;
};


#endif  //__PARSER__H__