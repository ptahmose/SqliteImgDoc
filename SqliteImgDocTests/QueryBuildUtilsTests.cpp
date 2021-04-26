#include "pch.h"

using namespace std;
using namespace SlImgDoc;

TEST(QueryBuildUtilsTests, UseDimCoordinateQueryCheckSql)
{
    CreateOptions opts;
    opts.dbFilename = ":memory:";
    opts.dimensions.emplace('C');
    opts.dimensions.emplace('S');
    opts.dimensions.emplace('T');
    auto db = IDbFactory::CreateNew(opts);
    auto internalDb = std::dynamic_pointer_cast<IInternalDb>(db);
    CDbDiscover discover(&internalDb->GetDb());
    discover.DoIt();
    auto docInfo = discover.GetDocInfo();

    CDimCoordinateQueryClause queryClause;
    queryClause.AddRangeClause('S', IDimCoordinateQueryClause::RangeClause{ 0,0 });
    queryClause.AddRangeClause('C', IDimCoordinateQueryClause::RangeClause{ 0,0 });
    queryClause.AddRangeClause('T', IDimCoordinateQueryClause::RangeClause{ 0,0 });
    auto query = QueryBuildUtils::Build(
        internalDb->GetDb(),
        *docInfo,
        &queryClause,
        nullptr);
    auto sqlStatement = query.getExpandedSQL();
}