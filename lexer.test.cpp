#include <cppunit/extensions/HelperMacros.h>

#include <stdio.h>
#include <string.h>

#include "lexer.h"

class LexerTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE( LexerTest );

  CPPUNIT_TEST( test_symbol );
  CPPUNIT_TEST( test_number );
  CPPUNIT_TEST( test_symbol );
  CPPUNIT_TEST( test_expr );
  CPPUNIT_TEST( test_relation );
  CPPUNIT_TEST( test_cond );
  CPPUNIT_TEST( test_logi );
  CPPUNIT_TEST( test_bit );
  CPPUNIT_TEST( test_brace );
  CPPUNIT_TEST( test_function );

  CPPUNIT_TEST_SUITE_END();

public:
  LexerTest() {}

  void setUp()
  {
    _t = new Lexer();
  }

  void tearDown()
  {
    delete _t;
  }

  void test_symbol()
  {
    _t->setBuffer(" hoge hige _hoge99 ");
    Lexer::Val val;
    val.i = 0;

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    CPPUNIT_ASSERT_MESSAGE("check symbol hoge", strcmp("hoge", val.sym) == 0);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    CPPUNIT_ASSERT_MESSAGE("check symbol hige", strcmp("hige", val.sym) == 0);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    CPPUNIT_ASSERT_MESSAGE("check symbol _hoge99", strcmp("_hoge99", val.sym) == 0);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::ENDOFFILE, val.type);
  }

  void test_number()
  {
    _t->setBuffer(" 999 2013 68030");
    Lexer::Val val;
    val.i = 0;

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    CPPUNIT_ASSERT_EQUAL(999, val.i);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    CPPUNIT_ASSERT_EQUAL(2013, val.i);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    CPPUNIT_ASSERT_EQUAL(68030, val.i);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::ENDOFFILE, val.type);
  }

  void test_assign()
  {
    _t->setBuffer("a = b + c");
    Lexer::Val val;
    val.i = 0;

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::ASSIGN, val.type);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::PLUS, val.type);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::ENDOFFILE, val.type);
  }

  void test_expr()
  {
    _t->setBuffer(" (41 + 5009 / 9) * 6502");
    Lexer::Val val;
    val.i = 0;

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::PARENTHESIS_OPEN, val.type);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    CPPUNIT_ASSERT_EQUAL(41, val.i);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::PLUS, val.type);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    CPPUNIT_ASSERT_EQUAL(5009, val.i);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::DIV, val.type);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    CPPUNIT_ASSERT_EQUAL(9, val.i);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::PARENTHESIS_CLOSE, val.type);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::MUL, val.type);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    CPPUNIT_ASSERT_EQUAL(6502, val.i);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::ENDOFFILE, val.type);
  }

  void test_relation()
  {
    _t->setBuffer(" 3 == 234; 3 != 5; 3 >= 5; 3 <= 5; 3 < 5; 3 > 5");
    Lexer::Val val;

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::EQUAL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    _t->getLex(&val);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NOT_EQUAL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    _t->getLex(&val);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::GREATER_EQUAL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    _t->getLex(&val);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::LESSER_EQUAL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    _t->getLex(&val);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::LESSER, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    _t->getLex(&val);

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::GREATER, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::NUMBER, val.type);
    _t->getLex(&val);

    CPPUNIT_ASSERT_EQUAL(Lexer::ENDOFFILE, val.type);
  }

  void test_cond()
  {
    _t->setBuffer("x = a ? b : c");
    Lexer::Val val;

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::ASSIGN, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::QUESTION, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::COLON, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);

    CPPUNIT_ASSERT_EQUAL(Lexer::ENDOFFILE, val.type);
  }

  void test_logi()
  {
    _t->setBuffer("a || b && c");
    Lexer::Val val;

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::LOGI_OR, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::LOGI_AND, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);

    CPPUNIT_ASSERT_EQUAL(Lexer::ENDOFFILE, val.type);
  }

  void test_bit()
  {
    _t->setBuffer("a | b & c ^ d << e >> f");
    Lexer::Val val;

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::OR, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::AND, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::XOR, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SHIFTL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SHIFTR, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
;
    CPPUNIT_ASSERT_EQUAL(Lexer::ENDOFFILE, val.type);
  }

  void test_brace() {
    _t->setBuffer("{a=b;}");
    Lexer::Val val;

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::BRACE_OPEN, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::ASSIGN, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::STATEMENT_CLOSE, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::BRACE_CLOSE, val.type);
    _t->getLex(&val);

    CPPUNIT_ASSERT_EQUAL(Lexer::ENDOFFILE, val.type);
  }

  void test_function() {
    _t->setBuffer("$(b){var a=b;}");
    Lexer::Val val;

    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::FUNCTION, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::PARENTHESIS_OPEN, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::PARENTHESIS_CLOSE, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::BRACE_OPEN, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::ASSIGN, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::SYMBOL, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::STATEMENT_CLOSE, val.type);
    _t->getLex(&val);
    CPPUNIT_ASSERT_EQUAL(Lexer::BRACE_CLOSE, val.type);
    _t->getLex(&val);

    CPPUNIT_ASSERT_EQUAL(Lexer::ENDOFFILE, val.type);
  }
private:
  Lexer *_t;
};

CppUnit::TestSuite* lexerTestSuite() { return LexerTest::suite(); }
