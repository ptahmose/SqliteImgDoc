#pragma once
#include <string>

class CmdlineOpts
{
private:
    std::string cziFilename;
public:
    CmdlineOpts();

    const std::string& GetCziFilename() const;
};