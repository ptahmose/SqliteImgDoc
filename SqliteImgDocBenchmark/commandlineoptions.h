#pragma once
#include <string>

class ParametersTestCase1
{
public:
    int zCount;
    int tCount;
};

class CommandLineOptions
{
private:
    std::string testDataFolder;
    ParametersTestCase1 paramsTestCase1;
public:
    CommandLineOptions();

    bool ParseArguments(int argc, char** argv);

    const ParametersTestCase1& GetParamsTestCase1() const;
private:
    void SetDefaults();
};