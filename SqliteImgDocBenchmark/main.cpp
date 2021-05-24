#include "commandlineoptions.h"
#include "testCase1.h"
#include "utils.h"
#include <iostream>
#include <windows.h>

using namespace std;
using namespace std::chrono;

static void RunTestCase1(const CommandLineOptions& cmdLineOptions);

int main(int argc, char** argv)
{
    SetConsoleOutputCP(CP_UTF8);

    CommandLineOptions cmdlineopts;
    cmdlineopts.ParseArguments(argc, argv);
    RunTestCase1(cmdlineopts);

    cout << u8"µäöüß";
}

static void PrintBenchmarkItem(const BenchmarkItem& item)
{
    cout << "Benchmark: " << item.benchmarkName << endl;

    auto detailLines = Utils::wrap(Utils::convertUtf8ToWchart(item.explanation).c_str(), 78);
    cout << "Details: " << endl;
    for (const auto& l : detailLines)
    {
        cout << Utils::convertToUtf8(l) << endl;
    }

    cout << "Runtime: " << (item.executionTime.count()) << "s" << endl;
}

/*static*/void RunTestCase1(const CommandLineOptions& cmdLineOptions)
{
    TestCase1 testcase1(cmdLineOptions.GetParamsTestCase1());

    PrintBenchmarkItem(testcase1.RunTest1());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest2());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest3());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest4());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest5());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest6());
    cout << endl;

    PrintBenchmarkItem(testcase1.RunTest7());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest8());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest9());
    cout << endl;
    PrintBenchmarkItem(testcase1.RunTest10());
    cout << endl;
}
