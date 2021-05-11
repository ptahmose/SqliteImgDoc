#pragma once

#include <string>
#include <chrono>

class BenchmarkItem
{
public:
    std::string benchmarkName;

    std::string explanation;

    std::chrono::duration<double> executionTime;
};