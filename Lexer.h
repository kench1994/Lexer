#include "Token.h"

#include <deque>
#include <cstdio>
#include <cctype>

#ifndef LEXER_H
#define LEXER_H

namespace ssql {

using std::istream;
using std::string;
using std::map;

#define BUF_SIZE 256
#define MAX_OP_SIZE 2

class LexError : std::exception {
public:
   LexError(string _msg) : msg(_msg) {}
   ~LexError() throw () {} // Updated
   const char* what() const throw() { return msg.c_str(); }
private:
    string msg;
};

class Lexer {
public:
    Lexer(istream &s) : stream(s) {
        line = 1;
        col = 1;
        initLookups();
    }

    bool isOp (char ch) const;

    Token next();

    int getCol() const {
        return col;
    }

    int getLine() const {
        return line;
    }

    // can't be read anymore i.e. peek == EOF
    bool isEnd() {
        peek = stream.peek();
        return peek == EOF;
    }

    // advance in the stream, return the read character
    char advance() {
        char last = stream.get();
        peek = stream.peek();
        col++;  // record column
        return last;
    }

    void retreat(char ch) {  // put ch back to stream
        stream.putback(ch);
        col--;  // record column
        peek = ch;
    }

    static map<string, Type> words;  // keywords
    // operators that can be determined by one character
    static map<char, Type> singleOp;
    static map<string, Type> ops;  // other operators

protected:

	/**
	 * @brief  尝试扫描 数值类型
	 * @date   2023-06-30
	 * @author huangchao
	 * @return Token 
	 *      01. 123456
            02. 123.456
            03. 123.456e3
            04. 123.456E3
            05. 123.456e+3
            06. 123.456E+3
            07. 123.456e-3
            08. 123.456E-3
			//下面的还不支持
            09. .1234
            10. .1234e3
            11. .1234E+3
            12. .1234e+3
            13. .1234E-3
            14. .1234e-3
	 */
	Token scanNumber();

private:
    static void initLookups();  // init the maps

    char peek;  // next character to read by advance()
    char buffer[BUF_SIZE];  // for keywords, operators etc.
	//2023年6月30日14:59:24 discard by huangc
	//i don't need get number numerical type even at all
    //int num_buffer;  // for accumulating numbers
    int line;  // line number
    int col; // column number
    istream &stream;  // stream for lexer
};

}

#endif