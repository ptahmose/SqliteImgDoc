#pragma once
#include <string>

class CmdlineOpts
{
private:
    std::string cziFilename;
    std::string dstFilename;
public:
    CmdlineOpts();

    const std::string& GetCziFilename() const;
    const std::string& GetDstFilename() const;
};