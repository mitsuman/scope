#include <ctype.h>
#include <stdio.h>

#include "lexer.h"

Lexer::Lexer() : _buf(0) {
}

Lexer::~Lexer() {
}

void Lexer::setBuffer(const char* src) {
  _buf = src;
}

static inline bool isfirstsym(char c) {
  return c == '_' || isalpha(c);
}

static inline bool issym(char c) {
  return isfirstsym(c) || isdigit(c);
}

void Lexer::getLex(Val *val) {
  if (!_buf) {
    val->type = ENDOFFILE;
    return;
  }

  char c = *_buf++;
  while (isspace(c)) {
    c = *_buf++;
  }

  if (!c) {
    _buf = 0;
    val->type = ENDOFFILE;
    return;
  }

  if (isdigit(c)) {
    int r = c - '0';
    while (c = *_buf) {
      if (!isdigit(c)) break;
      r = r * 10 + c - '0';
      _buf++;
    }
    val->type = NUMBER;
    val->i = r;
  } else if (isfirstsym(c)) {
    val->sym[0] = c;
    int p = 1;
    while (c = *_buf) {
      if (!issym(c)) break;
      val->sym[p] = c;
      _buf++;
      p++;
    }
    val->sym[p] = 0;
    val->type = SYMBOL;
  } else if (c == ';') {
    val->type = STATEMENT_CLOSE;
  } else if (c == '=' && _buf[0] == '=') {
    val->type = EQUAL;
    _buf++;
  } else if (c == '!' && _buf[0] == '=') {
    val->type = NOT_EQUAL;
    _buf++;
  } else if (c == '>' && _buf[0] == '=') {
    val->type = GREATER_EQUAL;
    _buf++;
  } else if (c == '<' && _buf[0] == '=') {
    val->type = LESSER_EQUAL;
    _buf++;
  } else if (c == '&' && _buf[0] == '&') {
    val->type = LOGI_AND;
    _buf++;
  } else if (c == '|' && _buf[0] == '|') {
    val->type = LOGI_OR;
    _buf++;
  } else if (c == '<' && _buf[0] == '<') {
    val->type = SHIFTL;
    _buf++;
  } else if (c == '>' && _buf[0] == '>') {
    val->type = SHIFTR;
    _buf++;
  } else if (c == '>') {
    val->type = GREATER;
  } else if (c == '<') {
    val->type = LESSER;
  } else if (c == '!') {
    val->type = NEGATIVE;
  } else if (c == '=') {
    val->type = ASSIGN;
  } else if (c == '+') {
    val->type = PLUS;
  } else if (c == '-') {
    val->type = MINUS;
  } else if (c == '*') {
    val->type = MUL;
  } else if (c == '/') {
    val->type = DIV;
  } else if (c == '%') {
    val->type = MOD;
  } else if (c == '|') {
    val->type = OR;
  } else if (c == '^') {
    val->type = XOR;
  } else if (c == '&') {
    val->type = AND;
  } else if (c == '(') {
    val->type = PARENTHESIS_OPEN;
  } else if (c == ')') {
    val->type = PARENTHESIS_CLOSE;
  } else if (c == '{') {
    val->type = BRACE_OPEN;
  } else if (c == '}') {
    val->type = BRACE_CLOSE;
  } else if (c == '[') {
    val->type = BRACKET_OPEN;
  } else if (c == ']') {
    val->type = BRACKET_CLOSE;
  } else if (c == '.') {
    val->type = DOT;
  } else if (c == ',') {
    val->type = COMMA;
  } else if (c == '?') {
    val->type = QUESTION;
  } else if (c == ':') {
    val->type = COLON;
  } else if (c == '$') {
    val->type = FUNCTION;
  } else {
    val->type = UNKNOWN;
  }
}
