#include "cmdlineopts.h"

CmdlineOpts::CmdlineOpts()
{
    /*this->cziFilename = R"(G:\Data\TestData\2014_02_05__16_39__0020-2.czi)";
    this->dstFilename = R"(E:\test.db)";*/
    this->cziFilename = R"(G:\Data\TestData\Example_TMA1_Zeb1_SPRR2_Ck19_S100-1-1-1-1.czi)";
    this->dstFilename = R"(E:\test2.db)";
}

const std::string& CmdlineOpts::GetCziFilename() const
{
    return this->cziFilename;
}

const std::string& CmdlineOpts::GetDstFilename() const
{
    return this->dstFilename;
}