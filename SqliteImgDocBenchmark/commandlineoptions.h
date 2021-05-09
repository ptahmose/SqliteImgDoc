#pragma once
#include <string>

class CommandLineOptions
{
private:
    std::string testDataFolder;
public:
    CommandLineOptions();

    bool ParseArguments(int argc, char** argv);
};