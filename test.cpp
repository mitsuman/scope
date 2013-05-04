#include <cppunit/ui/text/TestRunner.h>

extern CppUnit::TestSuite* lexerTestSuite();
extern CppUnit::TestSuite* parserTestSuite();
extern CppUnit::TestSuite* contextTestSuite();

int main() {
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(lexerTestSuite());
  runner.addTest(parserTestSuite());
  runner.addTest(contextTestSuite());
  bool retcode = runner.run();
  return !retcode;
}
