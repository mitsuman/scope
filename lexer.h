
#ifndef __LEXER_H
#define __LEXER_H

class Lexer {
public:
  Lexer();
  ~Lexer();

  const static int SYM_MAXLEN = 256;

  enum LEX {
    ENDOFFILE,
    UNKNOWN,

    SYMBOL,
    NUMBER,

    STATEMENT_CLOSE,

    // Dyadic Operator
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

    PLUS,
    MINUS,
    MUL,
    DIV,
    MOD,
    // End of Dyadic Operator

    ASSIGN,

    NEGATIVE,

    PARENTHESIS_OPEN,
    PARENTHESIS_CLOSE,

    BRACE_OPEN,
    BRACE_CLOSE,

    BRACKET_OPEN,
    BRACKET_CLOSE,

    DOT,
    COMMA,
    QUESTION,
    COLON,

    FUNCTION,
  };

  struct Val {
    LEX type;
    union {
      int i;
      char sym[SYM_MAXLEN];
    };
  };

  void setBuffer(const char* src);
  void getLex(Val *val);

private:
  const char *_buf;
};


#endif // __LEXER_H
