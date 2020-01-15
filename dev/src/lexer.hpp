#pragma once

#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
#include "command.cpp"
#include "console.cpp"
#include "syntax.cpp"



struct Options {

    std::unordered_map<std::string, std::string> map;

    Options();

    Options(std::unordered_map<std::string, std::string> opt);

    bool exists(std::string key);

    std::string get(std::string key);

    void set(std::string key, std::string value);

    int size();
};



class Lexer {

private:

    std::string sourcePath;
    std::string source;
    Options options;

    int index = -1;
    std::unordered_map<unsigned char, std::pair<int, std::vector<Token>>> nest = { { LPAREN, {} }, { LBRACK, {} }, { LBRACE, {} } };
    std::deque<Token> openParens;

public:

    Lexer();

    Lexer(std::string srcpath, std::string src, Options opt);

    std::vector<Token> getTokens();

private:

    Token scan();

    void checkParen(Token tk);

    void checkParenFinally();
};