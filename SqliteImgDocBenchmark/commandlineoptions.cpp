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

    this->paramsTestCase2.rowCount = 600;
    this->paramsTestCase2.columnCount = 600;
    this->paramsTestCase2.tileWidth = 1024;
    this->paramsTestCase2.tileHeight = 1024;
}

bool CommandLineOptions::ParseArguments(int argc, char** argv)
{
    return true;
}

const ParametersTestCase1& CommandLineOptions::GetParamsTestCase1() const
{
    return this->paramsTestCase1;
}

const ParametersTestCase2& CommandLineOptions::GetParamsTestCase2() const
{
    return this->paramsTestCase2;
}