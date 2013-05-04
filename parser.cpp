#include "lexer.h"
#include "node.h"
#include "parser.h"

Parser::Parser(int symbolOffset) : _lexer(0), _error(NOERROR), _symbolOffset(symbolOffset) {
}

Parser::~Parser() {
}

void Parser::setLexer(Lexer *lexer) {
  _lexer = lexer;
  _error = NOERROR;
  fetchVal();
}

Parser::ERROR Parser::getError() {
  return _error;
}

void Parser::error(ERROR error) {
  if (_error) return;

  _error = error;
}


// http://www.cs.man.ac.uk/~pjj/bnf/c_syntax.bnf
// http://timwhitlock.info/blog/2009/02/26/jparser-grammar/

Node* Parser::statement() {
  Node *r = 0;

  bool close = false;
  if (_val.type == Lexer::STATEMENT_CLOSE) {
    fetchVal();
    close = true;
  }

  if (_val.type == Lexer::BRACE_OPEN) {
    fetchVal();
    r = scope();

    Node *a = statement();
    if (a) {
      r = new Node(Node::STATEMENT, r, a);
    }
  } else if (_val.type == Lexer::BRACE_CLOSE) {
    if (close) {
      r = new Node(Node::CONTEXT);
    }
  } else {
    Node *a = expr();
    if (!a) return 0;

    Node *b = statement();
    if (!b) return a;

    r = new Node(Node::STATEMENT, a, b);
  }
  return r;
}

Node* Parser::scope() {
  Node *r = new Node(Node::SCOPE, statement(), 0);

  if (_val.type == Lexer::BRACE_CLOSE) {
    fetchVal();
  } else {
    error(NOTFOUND_BRACE_CLOSE);
  }
  return r;
}

Node* Parser::expr() {
  if (_val.type == Lexer::ENDOFFILE) return 0;

  Node *r = assignExpr();
  if (!r) {
    error(INVALID_EXPRESSION);
  }
  return r;
}

Node* Parser::assignExpr() {
  Node *r = condExpr();
  if (r) {
    if (_val.type == Lexer::ASSIGN) {
      fetchVal();
      Node *t = r;
      r = new Node(Node::ASSIGN, t, assignExpr());
    }
  }
  return r;
}

Node* Parser::condExpr() {
  Node *r = logiOrExpr();
  if (r) {
    if (_val.type == Lexer::QUESTION) {
      fetchVal();
      r = new Node(Node::COND, r, new Node(Node::NOP, logiOrExpr()));

      if (_val.type == Lexer::COLON) {
        fetchVal();
        r->b->b = condExpr();
      } else {
        error(INVALID_COND);
      }
    }
  }
  return r;
}

Node* Parser::logiOrExpr() {
  Node *r = logiAndExpr();
  if (r) {
    if (_val.type == Lexer::LOGI_OR) {
      fetchVal();
      r = new Node(Node::LOGI_OR, r, logiOrExpr());
    }
  }
  return r;
}

Node* Parser::logiAndExpr() {
  Node *r = orExpr();
  if (r) {
    if (_val.type == Lexer::LOGI_AND) {
      fetchVal();
      r = new Node(Node::LOGI_AND, r, logiAndExpr());
    }
  }
  return r;
}

Node* Parser::orExpr() {
  Node *r = xorExpr();
  if (r) {
    if (_val.type == Lexer::OR) {
      fetchVal();
      r = new Node(Node::OR, r, orExpr());
    }
  }
  return r;
}

Node* Parser::xorExpr() {
  Node *r = andExpr();
  if (r) {
    if (_val.type == Lexer::XOR) {
      fetchVal();
      r = new Node(Node::XOR, r, xorExpr());
    }
  }
  return r;
}

Node* Parser::andExpr() {
  Node *r = equalExpr();
  if (r) {
    if (_val.type == Lexer::AND) {
      fetchVal();
      r = new Node(Node::AND, r, andExpr());
    }
  }
  return r;
}

Node* Parser::equalExpr() {
  Node *r = relExpr();
  if (r) {
    int t = _val.type;
    if (t == Lexer::EQUAL || t == Lexer::NOT_EQUAL) {
      fetchVal();
      r = new Node((Node::TYPE) (t - Lexer::EQUAL + Node::EQUAL), r, relExpr());
    }
  }
  return r;
}

Node* Parser::relExpr() {
  Node *r = shiftExpr();
  if (r) {
    int t = _val.type;
    if (t >= Lexer::GREATER_EQUAL && t <= Lexer::LESSER) {
      fetchVal();
      r = new Node((Node::TYPE) (t - Lexer::EQUAL + Node::EQUAL), r, shiftExpr());
    }
  }
  return r;
}

Node* Parser::shiftExpr() {
  Node *r = addExpr();
  if (r) {
    int t = _val.type;
    if (t == Lexer::SHIFTL || t == Lexer::SHIFTR) {
      fetchVal();
      r = new Node((Node::TYPE) (t - Lexer::PLUS + Node::ADD), r, shiftExpr());
    }
  }
  return r;
}

Node* Parser::addExpr() {
  Node *r = mulExpr();
  if (r) {
    int t = _val.type;
    if (t == Lexer::PLUS || t == Lexer::MINUS) {
      fetchVal();
      r = new Node((Node::TYPE) (t - Lexer::PLUS + Node::ADD), r, addExpr());
    }
  }
  return r;
}

Node* Parser::mulExpr() {
  Node *r = unaryExpr();
  if (r) {
    int t = _val.type;
    if (t == Lexer::MUL || t == Lexer::DIV || t == Lexer::MOD) {
      fetchVal();
      r = new Node((Node::TYPE) (t - Lexer::MUL + Node::MUL), r, mulExpr());
    }
  }
  return r;
}

Node* Parser::unaryExpr() {
  Node *r = 0;
  int t = _val.type;
  if (t == Lexer::PLUS || t == Lexer::MINUS) {
    fetchVal();
    r = new Node(t == Lexer::PLUS ? Node::PLUS : Node::MINUS, propertyExpr());
  } else {
    r = propertyExpr();
  }
  return r;
}

Node* Parser::propertyExpr() {
  Node *r = functionExpr();
  if (r) {
    if (_val.type == Lexer::DOT) {
      fetchVal();
      r = new Node(Node::PROPERTY, r, propertyExpr());
      r = convertLeftHandSide(r);
    }
  }
  return r;
}

Node* Parser::functionExpr() {
  Node *r = primExpr();
  if (r && _val.type == Lexer::PARENTHESIS_OPEN) {
    r = new Node(Node::CALL, r, callArguments());
  }
  return r;
}

Node* Parser::convertLeftHandSide(Node *r) {
  if (r->b->type == Node::PROPERTY) {
    Node *t = r->b;
    r->b = t->a;
    t->a = convertLeftHandSide(r);
    r = t;
  }
  return r;
}

Node* Parser::primExpr() {
  Node *r = 0;
  if (_val.type == Lexer::PARENTHESIS_OPEN) {
    fetchVal();
    r = expr();
    if (_val.type != Lexer::PARENTHESIS_CLOSE) {
      error(NOTFOUND_PARENTHESIS_CLOSE);
    }
    fetchVal();
  } else if (_val.type == Lexer::BRACE_OPEN) {
    fetchVal();
    r = scope();
  } else if (_val.type == Lexer::FUNCTION) {
    fetchVal();
    Node *arg = arguments();
    r = expr();
    if (r) {
      if (r->type != Node::SCOPE) {
        r = new Node(Node::FUNCTION, r);
      } else {
        r->type = Node::FUNCTION;
      }
      r->b = arg;
    } else {
      error(INVALID_FUNCTION_SCOPE);
    }
  } else if (_val.type == Lexer::NUMBER) {
    r = new Node(Node::NUMBER, _val.i);
    fetchVal();
  } else if (_val.type == Lexer::SYMBOL) {
    r = new Node(Node::SYMBOL, makeSym(_val.sym));
    fetchVal();
  } else {
    error(INVALID_PRIME);
  }
  return r;
}

Node* Parser::arguments() {
  if (_val.type != Lexer::PARENTHESIS_OPEN) {
    return 0;
  }
  fetchVal();

  Node *r = 0;
  Node *p = 0;
  while (true) {
    if (_val.type == Lexer::SYMBOL) {
      int type = 0;
      int sym = makeSym(_val.sym);
      fetchVal();
      if (_val.type == Lexer::SYMBOL) {
        type = sym;
        sym = makeSym(_val.sym);
        fetchVal();
      }
      Node *e = new Node(Node::DECLARE);
      e->declare.type = type;
      e->declare.sym = sym;

      Node *t = new Node(Node::STATEMENT, e);
      if (p) {
        p->b = t;
      }
      if (!r) r = t;
      p = t;
    } else if (_val.type == Lexer::PARENTHESIS_CLOSE) {
      fetchVal();
      break;
    } else if (_val.type == Lexer::COMMA) {
      fetchVal();
      continue;
    } else {
      error(INVALID_ARGUMENTS);
      break;
    }
  }
  return r;
}

Node* Parser::callArguments() {
  if (_val.type != Lexer::PARENTHESIS_OPEN) {
    return 0;
  }
  fetchVal();

  Node *r = 0;
  Node *p = 0;
  while (true) {
    if (_val.type == Lexer::PARENTHESIS_CLOSE) {
      fetchVal();
      break;
    }

    Node *e = expr();
    if (!e) {
      error(INVALID_ARGUMENTS);
      break;
    }

    Node *t = new Node(Node::ARGUMENT, e);
    if (p) {
      p->b = t;
    }
    if (!r) r = t;
    p = t;

    if (_val.type == Lexer::COMMA) {
      fetchVal();
      continue;
    }
  }
  return r;
}

void Parser::fetchVal() {
  _lexer->getLex(&_val);
}

int Parser::makeSym(const char* sym) {
  int r = -1;
  std::map<std::string, int>::iterator s = _symbols.find(sym);
  if (s != _symbols.end()) {
    r = s->second;
  } else {
    r = _symbols.size() + _symbolOffset;
    _symbols.insert(std::pair<std::string, int>(sym, r));
  }
  return r;
}

void Parser::setSym(int i, const char* sym) {
  _symbols.insert(std::pair<std::string, int>(sym, i));
}
