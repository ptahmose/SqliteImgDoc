#include "cmdlineopts.h"
#include <iostream>
#include "../SqliteImgDoc/external/Interface.h"
#include "inc_libczi.h"
#include "utils.h"

using namespace SlImgDoc;
using namespace std;
using namespace libCZI;

int main(int argc, char** argv)
{
    CmdlineOpts opts;

    CreateStreamFromFile(CUtils::convertUtf8ToUCS2(opts.GetCziFilename()).c_str());
}