#ifndef __NODE_H
#define __NODE_H

class Val;
class Context;
class Node {
public:
  enum TYPE {
    NOP,

    NATIVE,

    // literal
    SYMBOL,
    NUMBER,

    SCOPE,
    STATEMENT,
    CONTEXT,

    ASSIGN,
    PROPERTY,

    FUNCTION,
    DECLARE,
    CALL,
    ARGUMENT,

    // ternary operator
    COND,

    // dyadic operator
    LOGI_OR,
    LOGI_AND,

    OR,
    XOR,
    AND,
    SHIFTL,
    SHIFTR,

    EQUAL,
    NOT_EQUAL,
    GREATER_EQUAL,
    LESSER_EQUAL,
    GREATER,
    LESSER,

    ADD,
    SUB,
    MUL,
    DIV,
    MOD,

    // unary operator
    PLUS,
    MINUS,

  };

  Node(TYPE t) : type(t) {}
  Node(TYPE t, Node *x) : type(t), a(x), b(0) {}
  Node(TYPE t, Node *x, Node *y) : type(t), a(x), b(y) {}
  Node(TYPE t, int x) : type(t), i(x) {}
  Node(TYPE t, void (*f)(Val*,Context*)) : type(t), native(f) {}

  TYPE type;
  union {
    int i;
    struct {
      Node *a;
      Node *b;
    };

    struct {
      int type;
      int sym;
    } declare;
    void (*native)(Val *out, Context *ctx);
  };
};

#endif //__NODE_H
