#include "cmdlineopts.h"

CmdlineOpts::CmdlineOpts()
{
    this->cziFilename = R"(G:\Data\TestData\2014_02_05__16_39__0020-2.czi)";
}

const std::string& CmdlineOpts::GetCziFilename() const
{
    return this->cziFilename;
}