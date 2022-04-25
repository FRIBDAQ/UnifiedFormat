#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;

int main(int argc, char** argv)
{
  CppUnit::TextUi::TestRunner   
               runner; // Control tests.
  CppUnit::TestFactoryRegistry& 
               registry(CppUnit::TestFactoryRegistry::getRegistry());

  runner.addTest(registry.makeTest());

  bool wasSucessful;
  try {
    wasSucessful = runner.run("",false);
  } 
  catch(string& rFailure) {
    cerr << "Caught a string exception from test suites.: \n";
    cerr << rFailure << endl;
    wasSucessful = false;
  }
  return !wasSucessful;
}

std::string uniqueName(std::string baseName)
{
    pid_t pid = getpid();
    char fullName[10000];
    sprintf(fullName, "%s_%d", baseName.c_str(), pid);
    return std::string(fullName);
}
void* gpTCLApplication(0);