#include "Lexer.h"

namespace ssql {

using std::size_t;

map<string, Type> Lexer::words;
map<char, Type> Lexer::singleOp;
map<string, Type> Lexer::ops;

Token Lexer::scanNumber()
{
	return Token();
}

void Lexer::initLookups()
{
	words["create"] = CREATE;
    words["table"] = TABLE;
	words["auto_increment"] = AUTO_INCREMENT;
    words["int"] = INT;
	words["unsigned"] = UNSIGNED;
    words["default"] = DEFAULT;
    words["primary"] = PRIMARY;
    words["key"] = KEY;
    words["insert"] = INSERT;
    words["into"] = INTO;
    words["values"] = VALUES;
    words["delete"] = DELETE;
    words["from"] = FROM;
    words["where"] = WHERE;
    words["select"] = SELECT;

    singleOp['+'] = PLUS;
    singleOp['-'] = MINUS;
    singleOp['*'] = MUL;
    singleOp['/'] = DIV;
    singleOp['('] = L_PAREN;
    singleOp[')'] = R_PAREN;
    singleOp[','] = COMMA;
    singleOp[';'] = SEMICOLON;

    ops["&&"] = AND;
    ops["||"] = OR;
    ops["!"] = NOT;
    ops["<"] = LT;
    ops[">"] = GT;
    ops["<>"] = NEQ;
    ops["="] = ASSIGN;
    ops["=="] = EQ;
    ops[">="] = GEQ;
    ops["<="] = LEQ;
    ops["+"] = PLUS;
    ops["-"] = MINUS;
    ops["*"] = MUL;
    ops["/"] = DIV;
    ops["("] = L_PAREN;
    ops[")"] = R_PAREN;
    ops[","] = COMMA;
    ops[";"] = SEMICOLON;
}

bool Lexer::isOp (char ch) const {
    return ch == '&' || ch == '|' || ch == '!' || ch == '<'
        || ch == '>' || ch == '=' || ch == '+' || ch == '-'
        || ch == '*' || ch == '/' || ch == '(' || ch == ')'
        || ch == ',' || ch == ';';
}

Token Lexer::next() {
    while(!isEnd()) {
        if (isdigit(peek)) {  // numbers
			int count = 0;
			memset(buffer, '\0', sizeof(buffer));
			do {
                buffer[count++] = advance();
            } while (count < BUF_SIZE \
			  && (isdigit(peek) || peek == '.' || peek == 'e' || peek == '+' || peek == 'e'));
			string str(buffer);
			return Token(NUM, str.c_str(), str.size());

            // num_buffer = 0;
            // do {
            //     num_buffer = num_buffer * 10 + advance() - '0';
            // } while (isdigit(peek) || );
            // return Token(NUM, &num_buffer, sizeof(int));
        } else if (isalpha(peek) || peek == '_') {  // keywords or identifilers
            memset(buffer, '\0', sizeof(buffer));
            int count = 0;
            do {
                buffer[count++] = advance();
            } while (count < BUF_SIZE && (isalnum(peek) || peek == '_'));

            if (count == BUF_SIZE) {
                throw LexError("Exceed maximun identifier length");
            }

            string str(buffer);

			//TODO:case insensitive
            for (size_t i = 0; i < str.size(); ++i) {
                str[i] = tolower(str[i]); // case insensitive
            }

            if (words.find(str) != words.end()) {  // keyword
                return Token(words[str]);
            } else {
                str = buffer;  // case sensitive for identifier
                return Token(ID, str.c_str(), str.size());
            }
        } else if (isspace(peek)) {  // white space
            if (advance() == '\n') {
                line++;
                col = 1;
            }
        } else if (singleOp.find(peek) != singleOp.end()) {
            // deterministic single character operators
            return Token(singleOp[advance()]);
        } else if (isOp(peek)) {  // operators
            memset(buffer, '\0', sizeof(buffer));
            int count = 0;
            do {
                buffer[count++] = advance();
            } while (isOp(peek) && count << MAX_OP_SIZE);

            string str(buffer);

            while (str.size() != 0 && ops.find(str) == ops.end()) { // too long
                char temp = str[str.size() - 1]; // last charactor
                str = str.substr(0, str.size() - 1);
                retreat(temp);
            }

            if (str.size() == 0) {
                throw LexError("Invalid operator");
            }

            return Token(ops[str]);
        } 
		//解析单引号包裹的文本
		else if(peek == '\'')
		{
			memset(buffer, '\0', sizeof(buffer));
            int count = 0;
            do {
                buffer[count++] = advance();
            } while (peek != '\'');
			//TODO: 有没有优雅点的方式
			buffer[count++] = advance();
            string str(buffer);

			return Token(CHARS, str.c_str(), str.size());

		}
		else {  // error
            string msg = "Invalid lexeme ";
            msg.push_back(advance());
            throw LexError(msg);
        }
    }

    // peek == EOF
    advance();
    return Token(END);  // $
}

}