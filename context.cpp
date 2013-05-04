#include "context.h"
#include "node.h"
#include "log.h"

#define e(x) #x
const char* Context::ERROR_string[] = { DEF_ERROR };
#undef e

Context::Context(Context *parent) : _parent(parent), _error(NOERROR) {
}

Context::~Context() {
}

Context::ERROR Context::getError() {
  return _error;
}

void Context::error(ERROR error) {
  if (_error) return;

  _error = error;
}

bool Context::setSym(int symi, Val *val) {
  Val* a = _getSym(symi, true);
  if (!a) return false;
  *a = *val;
  return true;
}

void (Context::*Context::UnaryOps[])(Val*, Val*) = {
  &Context::plus, &Context::minus,
};

void (Context::*Context::DyadicOps[])(Val*, Val*, Val*) = {
  &Context::logiOr, &Context::logiAnd,
  &Context::bitOr, &Context::bitXor, &Context::bitAnd,
  &Context::shiftl, &Context::shiftr,
  &Context::equal, &Context::notEqual,
  &Context::greaterEqual, &Context::lesserEqual,
  &Context::greater, &Context::lesser,
  &Context::add, &Context::sub,
  &Context::mul, &Context::div, &Context::mod,
};

void Context::eval(Val *out, Node *n) {
  do {
    _eval(out, n);
  } while (_isTrue(SYM_CONT));
}

bool Context::_isTrue(int sym) {
  std::map<int, Val>::iterator s = _vars.find(sym);
  return s != _vars.end() && s->second.type == Val::INTEGER && s->second._i;
}

void Context::_eval(Val *out, Node *n) {
  if (!n) return;
  if (_isTrue(SYM_BREAK)) return;

  if (Node::STATEMENT == n->type) {
    _eval(out, n->a);
    _eval(out, n->b);
  }

  else if (Node::SCOPE == n->type) {
    Context *ctx = new Context(this);
    ctx->eval(out, n->a);
  }

  else if (Node::CONTEXT == n->type) {
    out->type = Val::CONTEXT;
    out->ctx = this;
  }

  // unary operator
  else if (Node::PLUS <= n->type) {
    Val a;
    _eval(&a, n->a);
    (this->*UnaryOps[n->type - Node::PLUS])(out, &a);
  }

  // dyadic operator
  else if (Node::LOGI_OR <= n->type) {
    Val a, b;
    _eval(&a, n->a);
    _eval(&b, n->b);
    (this->*DyadicOps[n->type - Node::LOGI_OR])(out, &a, &b);
  }

  else if (Node::COND == n->type) {
    _eval(out, n->a);
    if (!_error) {
      _eval(out, out->_i ? n->b->a : n->b->b);
    }
  }

  else if (Node::NUMBER == n->type) {
    out->type = Val::INTEGER;
    out->_i = n->i;
  }

  else if (Node::SYMBOL == n->type) {
    switch (n->i) {
      case SYM_SELF:
        out->type = Val::CONTEXT;
        out->ctx = this;
        break;
      case SYM_PARENT:
        out->type = Val::CONTEXT;
        out->ctx = _parent;
        break;

      default:
        Val *a = _getSym(n->i, false);
        if (a) {
          *out = *a;
        } else {
          error(UNDEFINED_SYMBOL);
        }
    }
  }

  else if (Node::FUNCTION == n->type) {
    out->type = Val::FUNCTION;
    out->func.n = n;
    out->func.ctx = this;
  }

  else if (Node::CALL == n->type) {
    Val f;
    _eval(&f, n->a);

    if (f.type != Val::FUNCTION || !f.func.n) {
      error(NULL_FUNCTION);
      return;
    }
    Context *ctx = new Context(f.func.ctx);
    Node *p = n->b;
    Node *q = f.func.n->b;
    while (p && q) { // TODO:care the case of q(declare arg) is null
      Val v;
      _eval(&v, p->a);
      ctx->_vars[q->a->declare.sym] = v;
      p = p->b;
      q = q->b;
    }

    if (f.func.n->a->type != Node::NATIVE) {
      LOGF("call %p\n", f.func.n->a);
      ctx->eval(out, f.func.n->a);
    } else {
      LOGF("call(native) %p\n", f.func.n->a->native);
      f.func.n->a->native(out, ctx);
    }
  }

  else if (Node::ASSIGN == n->type) {
    Val *a = getSym(n->a);
    if (a) {
      _eval(a, n->b);
      *out = *a;
      LOGF("assign %d:%08x\n", a->type, a->_i);
    } else {
      error(INVALID_ASSIGNMENT);
    }
  }

  else if (Node::PROPERTY == n->type) {
    Val a;
    _eval(&a, n->a);
    if (a.type == Val::CONTEXT && a.ctx) {
      a.ctx->_eval(out, n->b);
    } else {
      error(NEED_CONTEXT);
    }
  }

  else {
    error(INVALID_NODE_TYPE);
  }
}

Val* Context::getSym(Node *n) {
  if (!n) return 0;

  if (n->type == Node::SYMBOL) {
    return _getSym(n->i, true);
  } else if (n->type == Node::PROPERTY) {
    if (n->b->type != Node::SYMBOL) {
      error(INVALID_PROPERTY);
      return 0;
    }

    Val a;
    _eval(&a, n->a);
    if (a.type != Val::CONTEXT || !a.ctx) {
      error(NEED_CONTEXT);
      return 0;
    }

    std::map<int, Val>::iterator i = a.ctx->_vars.find(n->b->i);
    if (i != a.ctx->_vars.end()) {
      return &i->second;
    }

    Val v;
    a.ctx->_vars.insert(std::pair<int, Val>(n->b->i, v));
    return &a.ctx->_vars.find(n->b->i)->second;
  }

  return 0;
}

Val* Context::_getSym(int symi, bool create) {
  std::map<int, Val>::iterator sym = _vars.find(symi);
  if (sym == _vars.end()) {
    if (_parent) {
      Val* r = _parent->_getSym(symi, false);
      if (r) {
        return r;
      }
    }

    if (!create) return 0;

    Val v;
    _vars.insert(std::pair<int, Val>(symi, v));
    sym = _vars.find(symi);
  }
  return &sym->second;
}


void Context::plus(Val *out, Val *a) {
  LOG();
  if (a->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::minus(Val *out, Val *a) {
  LOG();
  if (a->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = -a->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}


void Context::logiOr(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i || b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::logiAnd(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i && b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::bitOr(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i | b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::bitXor(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i ^ b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::bitAnd(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i & b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::shiftl(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i << b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::shiftr(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i >> b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}

void Context::equal(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i == b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::notEqual(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i != b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::greaterEqual(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i >= b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::lesserEqual(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i <= b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::greater(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i > b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::lesser(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i < b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}

void Context::add(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i + b->_i;
  } else if (a->type == Val::FUNCTION && b->type == Val::FUNCTION) {
    out->type = Val::FUNCTION;
    out->func.ctx = a->func.ctx; // TODO:where should we get ctx from correctly?
    out->func.n = new Node(Node::FUNCTION, new Node(Node::STATEMENT, a->func.n->a, b->func.n->a), a->func.n->b);
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::sub(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i - b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::mul(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i * b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::div(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i / b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
void Context::mod(Val *out, Val *a, Val *b) {
  LOG();
  if (a->type == Val::INTEGER && b->type == Val::INTEGER) {
    out->type = Val::INTEGER;
    out->_i = a->_i % b->_i;
  } else {
    error(UNSUPPORT_VAL_TYPE);
  }
}
