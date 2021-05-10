#include "testCase1.h"
#include <iostream>

using namespace std;
using namespace std::chrono;

int main(int argc, char** argv)
{
    TestCase1 testcase1(1000, 1000);
    
    BenchmarkItem item;
    item = testcase1.RunTest1();
    cout << item.benchmarkName << ": " << (item.executionTime.count()) << "s" << endl;
    item = testcase1.RunTest2();
    cout << item.benchmarkName << ": " << (item.executionTime.count()) << "s" << endl;
    item = testcase1.RunTest3();
    cout << item.benchmarkName << ": " << (item.executionTime.count()) << "s" << endl;
    item = testcase1.RunTest4();
    cout << item.benchmarkName << ": " << (item.executionTime.count()) << "s" << endl;
    item = testcase1.RunTest5();
    cout << item.benchmarkName << ": " << (item.executionTime.count()) << "s" << endl;
    item = testcase1.RunTest6();
    cout << item.benchmarkName << ": " << (item.executionTime.count()) << "s" << endl;
}