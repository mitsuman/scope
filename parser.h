#ifndef __PARSER_H
#define __PARSER_H

class Node;
class Lexer;

#include <map>
#include <string>

class Parser {
public:
  Parser(int symbolOffset = 0);
  ~Parser();

  void setLexer(Lexer *lexer);

  enum ERROR {
    NOERROR,
    NOTFOUND_PARENTHESIS_CLOSE,
    NOTFOUND_BRACE_CLOSE,
    NOTFOUND_BRACKET_CLOSE,
    NOTFOUND_STATEMENT_CLOSE,
    INVALID_EXPRESSION,
    INVALID_PRIME,
    INVALID_COND,
    INVALID_ARGUMENTS,
    INVALID_CALL_ARGUMENTS,
    INVALID_FUNCTION_SCOPE,
  };
  ERROR getError();
  void error(ERROR error);

  Node* statement();
  Node* expr();
  Node* scope();
  Node* assignExpr();
  Node* condExpr();
  Node* logiOrExpr();
  Node* logiAndExpr();
  Node* orExpr();
  Node* xorExpr();
  Node* andExpr();
  Node* shiftExpr();
  Node* equalExpr();
  Node* relExpr();
  Node* addExpr();
  Node* mulExpr();
  Node* unaryExpr();
  Node* functionExpr();
  Node* primExpr();
  Node* propertyExpr();
  Node* arguments();
  Node* callArguments();

  int makeSym(const char* sym);
  void setSym(int i, const char* sym);

private:
  void fetchVal();
  static Node* convertLeftHandSide(Node *r);

  Lexer *_lexer;
  Lexer::Val _val;

  int _symbolOffset;
  std::map<std::string, int> _symbols;

  ERROR _error;
};

#endif //__PARSER_H
