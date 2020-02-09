#include "cmdlineopts.h"
#include <cxxopts.hpp>

// https://github.com/jarro2783/cxxopts
// consider https://agauniyal.github.io/rang/

using namespace std;

CmdlineOpts::CmdlineOpts()
{
    /*this->cziFilename = R"(G:\Data\TestData\2014_02_05__16_39__0020-2.czi)";
    this->dstFilename = R"(E:\test.db)";*/
}

bool CmdlineOpts::ParseArguments(int argc, char** argv)
{
    cxxopts::Options options("ConvCZI", "convert CZI to SqliteImgDoc");
    options.add_options()
        ("i,input", "source CZI-file", cxxopts::value<std::string>())
        ("o,output", "output filename", cxxopts::value<std::string>())
        ("h,help", "print help and exit");
    const auto result = options.parse(argc, argv);

    if (result.count("help")>0)
    {
        this->PrintHelp();
        return false;
    }

    if (result.count("input")!=1)
    {
        cerr << "No input specified";
        return false;
    }

    if (result.count("output") != 1)
    {
        cerr << "No output specified";
        return false;
    }

    this->cziFilename = result["input"].as<string>();
    this->dstFilename = result["output"].as<string>();
    return true;
}

const std::string& CmdlineOpts::GetCziFilename() const
{
    return this->cziFilename;
}

const std::string& CmdlineOpts::GetDstFilename() const
{
    return this->dstFilename;
}

void CmdlineOpts::PrintHelp()
{
    static const char helpTxt[] =
        "Usage: ConvCZI --input <source> --output <output>\n"
        "\n"
        "-i, --input FILE    specifies the input CZI-file\n"
        "-o, --output FILE   specifies the output file\n"
        "-h, --help          show help and exit\n";
    cout << helpTxt;
}