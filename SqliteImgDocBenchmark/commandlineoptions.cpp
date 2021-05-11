#include "commandlineoptions.h"
#include <cxxopts.hpp>

using namespace std;

CommandLineOptions::CommandLineOptions()
{
    this->SetDefaults();
}

void CommandLineOptions::SetDefaults()
{
    this->paramsTestCase1.zCount = 1000;
    this->paramsTestCase1.tCount = 1000;
}

bool CommandLineOptions::ParseArguments(int argc, char** argv)
{
    return true;
}

const ParametersTestCase1& CommandLineOptions::GetParamsTestCase1() const
{
    return this->paramsTestCase1;
}