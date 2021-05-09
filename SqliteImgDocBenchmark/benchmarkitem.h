#pragma once

#include <string>
#include <chrono>

class BenchmarkItem
{
public:
    std::string benchmarkName;

    std::chrono::duration<double> executionTime;
};