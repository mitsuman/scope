#include <cppunit/extensions/HelperMacros.h>

#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "node.h"
#include "context.h"

#define TEST_EXPR(NAME,F,A) void test_##NAME() {   \
_t->setBuffer(F);_p->setLexer(_t); \
Val v; \
_c->eval(&v, _p->expr()); \
if (Context::NOERROR != _c->getError()) { \
 char s[128]; sprintf(s, "ERROR %s", Context::ERROR_string[_c->getError()]); \
 CPPUNIT_FAIL(s); \
} \
CPPUNIT_ASSERT_EQUAL_MESSAGE("TYPE", Val::INTEGER, v.type);                     \
CPPUNIT_ASSERT_EQUAL_MESSAGE("VALUE", A, v._i);                                      \
}

#define TEST_STATEMENT(NAME,F,A) void test_##NAME() {   \
_p->setSym(Context::SYM_SELF, "__self"); \
_p->setSym(Context::SYM_PARENT, "__parent"); \
_p->setSym(Context::SYM_BREAK, "__break"); \
_p->setSym(Context::SYM_CONT, "__cont"); \
_t->setBuffer(F);_p->setLexer(_t); \
Val v; \
_c->eval(&v, _p->statement()); \
if (Context::NOERROR != _c->getError()) { \
 char s[128]; sprintf(s, "ERROR %s", Context::ERROR_string[_c->getError()]); \
 CPPUNIT_FAIL(s); \
} \
CPPUNIT_ASSERT_EQUAL_MESSAGE("TYPE", Val::INTEGER, v.type);                 \
CPPUNIT_ASSERT_EQUAL_MESSAGE("VALUE", A, v._i);                              \
}

#define ERROR_STATEMENT(NAME,F) void test_##NAME() {   \
_t->setBuffer(F);_p->setLexer(_t); \
Val v; \
_c->eval(&v, _p->statement()); \
CPPUNIT_ASSERT(Context::NOERROR != _c->getError()); \
}


class ContextTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE( ContextTest );

  CPPUNIT_TEST( test_prim );
  CPPUNIT_TEST( test_add );
  CPPUNIT_TEST( test_add_2 );
  CPPUNIT_TEST( test_add_mul );
  CPPUNIT_TEST( test_sub_div );
  CPPUNIT_TEST( test_paren );
  CPPUNIT_TEST( test_paren_2 );
  CPPUNIT_TEST( test_assign );
  CPPUNIT_TEST( test_assign_2 );
  CPPUNIT_TEST( test_bit );
  CPPUNIT_TEST( test_bit_2 );
  CPPUNIT_TEST( test_shift );

  CPPUNIT_TEST( test_statement );
  CPPUNIT_TEST( test_statement_2 );
  CPPUNIT_TEST( test_scope );
  CPPUNIT_TEST( test_cond );
  CPPUNIT_TEST( test_cond_2 );
  CPPUNIT_TEST( test_cond_3 );
  CPPUNIT_TEST( test_scope_2 );
  CPPUNIT_TEST( test_scope_3 );
  CPPUNIT_TEST( test_function );
  CPPUNIT_TEST( test_function_2 );
  CPPUNIT_TEST( test_function_3 );
  CPPUNIT_TEST( test_function_4 );
  CPPUNIT_TEST( test_context );
  CPPUNIT_TEST( test_context_2 );
  CPPUNIT_TEST( test_context_3 );
  CPPUNIT_TEST( test_context_4 );
  CPPUNIT_TEST( test_context_5 );
  CPPUNIT_TEST( test_context_6 );
  CPPUNIT_TEST( test_context_7 );
  CPPUNIT_TEST( test_context_8 );
  CPPUNIT_TEST( test_inherit_0 );
  CPPUNIT_TEST( test_inherit_1 );
  CPPUNIT_TEST( test_native );
  CPPUNIT_TEST( test_self );
  CPPUNIT_TEST( test_self_2 );
  CPPUNIT_TEST( test_self_3 );
  CPPUNIT_TEST( test_self_4 );
  CPPUNIT_TEST( test_self_5 );
  CPPUNIT_TEST( test_control );
  CPPUNIT_TEST( test_control_2 );
  CPPUNIT_TEST( test_control_3 );

  CPPUNIT_TEST_SUITE_END();

public:
  ContextTest() {}

  void setUp()
  {
    _t = new Lexer();
    _p = new Parser(Context::RESERVED_SYM_MAX);
    _c = new Context(0);
  }

  void tearDown()
  {
    delete _p;
    delete _t;
    delete _c;
  }

  TEST_EXPR(prim, "5", 5)
  TEST_EXPR(add, "5+3", 8)
  TEST_EXPR(sub, "53-32", 11)
  TEST_EXPR(add_2,"5+9+8", 22)
  TEST_EXPR(add_mul, "1+2*3", 7)
  TEST_EXPR(sub_div, "1-9/3", -2)
  TEST_EXPR(paren, "(1+2)*3", 9)
  TEST_EXPR(paren_2, "((1+2)*(3-1))", 6)
  TEST_EXPR(assign, "a=(1+2)*3", 9)
  TEST_EXPR(assign_2, "a=b=3", 3)
  TEST_EXPR(bit, "3|5", 7)
  TEST_EXPR(bit_2, "3&1|5", 5)
  TEST_EXPR(shift, "1 << 1+1", 4)

  TEST_STATEMENT(statement, "a=b=3;a+b", 6)
  TEST_STATEMENT(statement_2, "a=3;b=9;a+b-5", 7)
  TEST_STATEMENT(scope, "a=3;{a=2}a", 2)
  TEST_STATEMENT(cond, "1?2:3", 2)
  TEST_STATEMENT(cond_2, "a=1;b=2;a>b ? a : 999", 999)
  TEST_STATEMENT(cond_3, "a=3;b=2; a - 1 == b ? a * a : b", 9)
  TEST_STATEMENT(scope_2, "a={b=2;b*2};a+1", 5)
  ERROR_STATEMENT(scope_3, "a={b=2;b*2};b+1")
  TEST_STATEMENT(function, "f=$(a,b){a+b};f(1,2)", 3)
  TEST_STATEMENT(function_2, "f=$(a,b) a+b; f(1,2)", 3)
  TEST_STATEMENT(function_3, "f=$(a,b){g=$(a,b){a*b};a+b+g(a,b)} f(2,3)", 11)
  TEST_STATEMENT(function_4, "f=$(a){a>0?a+f(a-1):0} f(5)", 5+4+3+2+1)
  TEST_STATEMENT(context, "f={a=1;b=3;}f.a", 1)
  TEST_STATEMENT(context_2, "f={a=1;b=3;}; g={a=7;b=12;c=32;}f.a+g.b", 13)
  TEST_STATEMENT(context_3, "f={a=1;b=3;}; g=$(a){a.a+a.b}; g(f)", 4)
  TEST_STATEMENT(context_4, "a=0;b=0;{a=1;b=3;}; a-b", -2)
  TEST_STATEMENT(context_5, "a={x=2;y=3} a", 3)
  TEST_STATEMENT(context_6, "a=${x=2;y=3;} a().y", 3)
  TEST_STATEMENT(context_7, "a=${x=2;y=3;} b=a();b.x+b.y", 5)
  TEST_STATEMENT(context_8, "a={b=0;f=$(x){b=x*x};}; (a.f)(3); a.b", 3*3)

  TEST_STATEMENT(inherit_0, "A=${x=2;y=3;}; B=A+${z=10;}; b=B();b.x+b.z", 12)
  TEST_STATEMENT(inherit_1, "A=$(i){x=i;y=i*2;}; B=A+$(i){z=i*3;}; b=B(5);b.x+b.y+b.z", 5+10+15)

  static void hoge_func(Val *out, Context *ctx) {
    out->type = Val::INTEGER;
    out->_i = 68000;
  }

  void test_native() {
    Val hoge;
    hoge.type = Val::FUNCTION;
    hoge.func.n = new Node(Node::FUNCTION, new Node(Node::NATIVE, hoge_func));
    hoge.func.ctx = _c;
    int s_hoge = _p->makeSym("hoge");
    _c->setSym(s_hoge, &hoge);

    _t->setBuffer("hoge()"); _p->setLexer(_t);

    Val v;
    _c->eval(&v, _p->statement());

    CPPUNIT_ASSERT_EQUAL(Context::NOERROR, _c->getError());
    CPPUNIT_ASSERT_EQUAL(Val::INTEGER, v.type);
    CPPUNIT_ASSERT_EQUAL(68000, v._i);
  }

  TEST_STATEMENT(self, "a=1;b=3;__self.b", 3)
  TEST_STATEMENT(self_2, "a=1;b=3;c=__self;c.a", 1)
  TEST_STATEMENT(self_3, "a=7;b=9;c=__self;f=$(a){a.a};f(c)", 7)
  TEST_STATEMENT(self_4, "a=7;b={__self.a=2;k=__parent.a + 1;};b.k", 8)
  TEST_STATEMENT(self_5, "a=7;b={__self.a=4;c={__self.a=1;d=__self.__parent.__parent.a;};};b.c.d+b.a+b.c.a", 7+4+1)

  TEST_STATEMENT(control, "a=0;n=0;{n=n+1;a=a+n;__self.__cont=n<10;};a", 55)
  TEST_STATEMENT(control_2, "a=0;{a=3;__break=1;a=5;} a", 3)
  TEST_STATEMENT(control_3, "a=0;{a=3;__parent.__break=1;a=5} 9", 5)
private:
  Lexer *_t;
  Parser *_p;
  Context *_c;
};

CppUnit::TestSuite* contextTestSuite() { return ContextTest::suite(); }
