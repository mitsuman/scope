#ifndef __CONTEXT_H
#define __CONTEXT_H

#include <map>

class Node;
class Context;

class Val {
public:
  enum TYPE {
    INTEGER,
    STRING,
    FUNCTION,
    CONTEXT,
  };
  TYPE type;

  union {
    int _i;
    struct {
      Node *n;
      Context *ctx;
    } func;
    Context *ctx;
  };
};

class Context {
public:
  Context(Context *parent);
  ~Context();

  void eval(Val *out, Node *n);
  bool setSym(int sym, Val *val);

#define DEF_ERROR            \
  e(NOERROR),                \
    e(UNSUPPORT_VAL_TYPE),   \
    e(UNDEFINED_SYMBOL),     \
    e(INVALID_ASSIGNMENT),   \
                             \
  e(INVALID_NODE_TYPE),      \
    e(NULL_FUNCTION),        \
    e(NEED_CONTEXT),         \
    e(INVALID_PROPERTY)

#define e(x) x
  enum ERROR { DEF_ERROR };
#undef e

  static const char* ERROR_string[];


  ERROR getError();
  void error(ERROR error);

  enum RESERVED_SYMBOL {
    SYM_SELF,
    SYM_PARENT,
    SYM_BREAK,
    SYM_CONT,
    RESERVED_SYM_MAX,
  };
private:
  void _eval(Val *out, Node *n);
  Val* getSym(Node *n);
  Val* _getSym(int sym, bool create = false);

  bool _isTrue(int sym);

  static void (Context::*UnaryOps[])(Val *out, Val *a);
  void plus(Val *out, Val *a);
  void minus(Val *out, Val *a);

  static void (Context::*DyadicOps[])(Val *out, Val *a, Val *b);
  void logiOr(Val *out, Val *a, Val *b);
  void logiAnd(Val *out, Val *a, Val *b);
  void bitOr(Val *out, Val *a, Val *b);
  void bitXor(Val *out, Val *a, Val *b);
  void bitAnd(Val *out, Val *a, Val *b);
  void shiftl(Val *out, Val *a, Val *b);
  void shiftr(Val *out, Val *a, Val *b);

  void equal(Val *out, Val *a, Val *b);
  void notEqual(Val *out, Val *a, Val *b);
  void greaterEqual(Val *out, Val *a, Val *b);
  void lesserEqual(Val *out, Val *a, Val *b);
  void greater(Val *out, Val *a, Val *b);
  void lesser(Val *out, Val *a, Val *b);

  void add(Val *out, Val *a, Val *b);
  void sub(Val *out, Val *a, Val *b);
  void mul(Val *out, Val *a, Val *b);
  void div(Val *out, Val *a, Val *b);
  void mod(Val *out, Val *a, Val *b);

  std::map<int, Val> _vars;
  Context *_parent;
  Val _result;

  ERROR _error;
};

#endif //__CONTEXT_H
