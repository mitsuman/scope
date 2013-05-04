#include <cppunit/extensions/HelperMacros.h>

#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "node.h"

class ParserTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE( ParserTest );

  CPPUNIT_TEST( test_prim );
  CPPUNIT_TEST( test_add );
  CPPUNIT_TEST( test_add_2 );
  CPPUNIT_TEST( test_add_mul );
  CPPUNIT_TEST( test_sub_div );
  CPPUNIT_TEST( test_paren );
  CPPUNIT_TEST( test_paren_2 );
  CPPUNIT_TEST( test_assign );
  CPPUNIT_TEST( test_assign_2 );
  CPPUNIT_TEST( test_unary );
  CPPUNIT_TEST( test_unary_2 );
  CPPUNIT_TEST( test_property );
  CPPUNIT_TEST( test_property_2 );
  CPPUNIT_TEST( test_property_3 );
  CPPUNIT_TEST( test_logi );
  CPPUNIT_TEST( test_logi_2 );
  CPPUNIT_TEST( test_bit );
  CPPUNIT_TEST( test_cond );
  CPPUNIT_TEST( test_shift );
  CPPUNIT_TEST( test_symbol );
  CPPUNIT_TEST( test_statement );
  CPPUNIT_TEST( test_function );
  CPPUNIT_TEST( test_function_2 );
  CPPUNIT_TEST( test_function_3 );
  CPPUNIT_TEST( test_context );
  CPPUNIT_TEST( test_prepared_symbol );

  CPPUNIT_TEST_SUITE_END();

public:
  ParserTest() {}

  void setUp()
  {
    _t = new Lexer();
    _p = new Parser();
  }

  void tearDown()
  {
    delete _p;
    delete _t;
  }

  void test_prim()
  {
    _t->setBuffer("5 3");
    _p->setLexer(_t);

    Node *n = _p->primExpr();
    CPPUNIT_ASSERT(0 != n);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->type);

    n = _p->primExpr();
    CPPUNIT_ASSERT(0 != n);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->type);
  }

  void test_add()
  {
    _t->setBuffer("5+3");
    _p->setLexer(_t);

    Node *n = _p->addExpr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->type);
    CPPUNIT_ASSERT_EQUAL(5, n->a->i);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->type);
    CPPUNIT_ASSERT_EQUAL(3, n->b->i);
  }

  void test_sub()
  {
    _t->setBuffer("a-b");
    _p->setLexer(_t);

    Node *n = _p->addExpr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::SUB, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->a->type);
    CPPUNIT_ASSERT_EQUAL(0, n->a->i);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->type);
    CPPUNIT_ASSERT_EQUAL(1, n->b->i);
  }

  void test_add_2()
  {
    _t->setBuffer("5+a+b");
    _p->setLexer(_t);

    Node *n = _p->addExpr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->type);
    CPPUNIT_ASSERT_EQUAL(5, n->a->i);
    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->b->type);
  }

  void test_add_mul()
  {
    _t->setBuffer("1+2*3");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::MUL, n->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->b->type);
  }

  void test_sub_div()
  {
    _t->setBuffer("1-2/3");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::SUB, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::DIV, n->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->b->type);
  }

  void test_paren()
  {
    _t->setBuffer("(1+2)*3");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::MUL, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->type);
  }

  void test_paren_2()
  {
    _t->setBuffer("((1+2)*(3-1))");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::MUL, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::SUB, n->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->b->type);
  }

  void test_assign()
  {
    _t->setBuffer("a=(1+2)*3");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::ASSIGN, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::MUL, n->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->a->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->a->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->b->type);
  }

  void test_assign_2()
  {
    _t->setBuffer("a=b=3");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::ASSIGN, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::ASSIGN, n->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->b->type);
  }

  void test_unary()
  {
    _t->setBuffer("-(1+2)");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::MINUS, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->b->type);
  }

  void test_unary_2()
  {
    _t->setBuffer("+2");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::PLUS, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->type);
  }

  void test_property()
  {
    _t->setBuffer("a.b");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::PROPERTY, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->type);
  }

  void test_property_2()
  {
    _t->setBuffer("1.2.3");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::PROPERTY, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->type);
    CPPUNIT_ASSERT_EQUAL(3, n->b->i);
    CPPUNIT_ASSERT_EQUAL(Node::PROPERTY, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->a->type);
    CPPUNIT_ASSERT_EQUAL(2, n->a->b->i);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->b->type);
    CPPUNIT_ASSERT_EQUAL(1, n->a->a->i);
  }

  void test_property_3()
  {
    _t->setBuffer("1.2.3.4");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::PROPERTY, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->type);
    CPPUNIT_ASSERT_EQUAL(4, n->b->i);
    CPPUNIT_ASSERT_EQUAL(Node::PROPERTY, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->b->type);
    CPPUNIT_ASSERT_EQUAL(3, n->a->b->i);
    CPPUNIT_ASSERT_EQUAL(Node::PROPERTY, n->a->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->a->b->type);
    CPPUNIT_ASSERT_EQUAL(2, n->a->a->b->i);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->a->a->a->type);
    CPPUNIT_ASSERT_EQUAL(1, n->a->a->a->i);
  }

  void test_logi()
  {
    _t->setBuffer("a||b&&c");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::LOGI_OR, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::LOGI_AND, n->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->b->type);
  }

  void test_logi_2()
  {
    _t->setBuffer("a - c == d + e");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::EQUAL, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::SUB, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->a->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->a->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->b->type);
  }

  void test_bit()
  {
    _t->setBuffer("a|b&c");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::OR, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::AND, n->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->b->type);
  }

  void test_cond()
  {
    _t->setBuffer("f=a==b?c:d+e");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::ASSIGN, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::COND, n->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::EQUAL, n->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NOP, n->b->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->b->b->b->type);
  }

  void test_shift()
  {
    _t->setBuffer("a << b + e");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::SHIFTL, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->b->type);
  }

  void test_symbol()
  {
    _t->setBuffer("a + b + c + a");
    _p->setLexer(_t);

    Node *n = _p->expr();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->a->type);
    CPPUNIT_ASSERT_EQUAL(0, n->a->i);
    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->a->type);
    CPPUNIT_ASSERT_EQUAL(1, n->b->a->i);
    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->b->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->b->a->type);
    CPPUNIT_ASSERT_EQUAL(2, n->b->b->a->i);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->b->b->type);
    CPPUNIT_ASSERT_EQUAL(0, n->b->b->b->i);
  }

  void test_statement()
  {
    _t->setBuffer("a = 2; b = 3; a + b;");
    _p->setLexer(_t);

    Node *n = _p->statement();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::STATEMENT, n->type);
    CPPUNIT_ASSERT(n->a != 0);
    CPPUNIT_ASSERT_EQUAL(Node::ASSIGN, n->a->type);
    CPPUNIT_ASSERT(n->b != 0);
    CPPUNIT_ASSERT_EQUAL(Node::STATEMENT, n->b->type);
    CPPUNIT_ASSERT(n->b->a != 0);
    CPPUNIT_ASSERT_EQUAL(Node::ASSIGN, n->b->a->type);
    CPPUNIT_ASSERT(n->b->b != 0);
    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->b->b->type);
  }

  void test_function()
  {
    _t->setBuffer("f=$(a,b){a+b};");
    _p->setLexer(_t);

    Node *n = _p->statement();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::ASSIGN, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::FUNCTION, n->b->type);

    CPPUNIT_ASSERT_EQUAL(Node::STATEMENT, n->b->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::DECLARE, n->b->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::STATEMENT, n->b->b->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::DECLARE, n->b->b->b->a->type);

    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->b->a->type);
  }

  void test_function_2()
  {
    _t->setBuffer("f=${a+b};");
    _p->setLexer(_t);

    Node *n = _p->statement();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::ASSIGN, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::FUNCTION, n->b->type);

    CPPUNIT_ASSERT_EQUAL(Node::ADD, n->b->a->type);
  }

  void test_function_3()
  {
    _t->setBuffer("f=$(a,b){a+b};f(1,2)");
    _p->setLexer(_t);

    Node *n = _p->statement();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::STATEMENT, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::ASSIGN, n->a->type);

    CPPUNIT_ASSERT_EQUAL(Node::CALL, n->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::ARGUMENT, n->b->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::ARGUMENT, n->b->b->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::NUMBER, n->b->b->b->a->type);
  }

  void test_context()
  {
    _t->setBuffer("h={b=2;}");
    _p->setLexer(_t);

    Node *n = _p->statement();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::ASSIGN, n->type);

    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::SCOPE, n->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::STATEMENT, n->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::ASSIGN, n->b->a->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::CONTEXT, n->b->a->b->type);
  }

  void test_prepared_symbol()
  {
    _t->setBuffer("a?b.break:b.retry");
    _p->setLexer(_t);
    int s_break = _p->makeSym("break");
    int s_retry = _p->makeSym("retry");

    Node *n = _p->statement();

    CPPUNIT_ASSERT_EQUAL(Parser::NOERROR, _p->getError());
    CPPUNIT_ASSERT_EQUAL(Node::COND, n->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::NOP, n->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::PROPERTY, n->b->a->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->a->b->type);
    CPPUNIT_ASSERT_EQUAL(s_break, n->b->a->b->i);
    CPPUNIT_ASSERT_EQUAL(Node::PROPERTY, n->b->b->type);
    CPPUNIT_ASSERT_EQUAL(Node::SYMBOL, n->b->b->b->type);
    CPPUNIT_ASSERT_EQUAL(s_retry, n->b->b->b->i);
  }

private:
  Lexer *_t;
  Parser *_p;
};

CppUnit::TestSuite* parserTestSuite() { return ParserTest::suite(); }
