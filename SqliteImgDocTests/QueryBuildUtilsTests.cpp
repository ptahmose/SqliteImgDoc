#include "pch.h"
#include <regex>


using namespace std;
using namespace SlImgDoc;

/// Removes the multiple consecutive blanks in the specified string. So, e.g. "AB CD    D    " becomes "AB CD D ".
/// \param str The string.
/// \returns String with multiple consecutive blanks converted to a single blank.
static string RemoveMultipleConsecutiveBlanks(string str)
{
    regex spaceRegex(R"(\s+)");
    return regex_replace(str, spaceRegex, " ");
}

TEST(QueryBuildUtilsTests, UseDimCoordinateQueryCheckSqlStatement1)
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
    auto sqlCommand = RemoveMultipleConsecutiveBlanks(query.getExpandedSQL());
    EXPECT_STRCASEEQ(sqlCommand.c_str(), "SELECT Pk FROM TILESINFO WHERE ((Dim_C= 0)) AND ((Dim_S= 0)) AND ((Dim_T= 0))");
}

TEST(QueryBuildUtilsTests, UseDimCoordinateQueryCheckSqlStatement2)
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
    queryClause.AddRangeClause('S', IDimCoordinateQueryClause::RangeClause{ 1,1 });
    queryClause.AddRangeClause('C', IDimCoordinateQueryClause::RangeClause{ 2,2 });
    queryClause.AddRangeClause('T', IDimCoordinateQueryClause::RangeClause{ 3,3 });
    auto query = QueryBuildUtils::Build(
        internalDb->GetDb(),
        *docInfo,
        &queryClause,
        nullptr);
    auto sqlCommand = RemoveMultipleConsecutiveBlanks(query.getExpandedSQL());
    EXPECT_STRCASEEQ(sqlCommand.c_str(), "SELECT Pk FROM TILESINFO WHERE ((Dim_C= 2)) AND ((Dim_S= 1)) AND ((Dim_T= 3))");
}

TEST(QueryBuildUtilsTests, UseDimCoordinateQueryCheckSqlStatement3)
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
    queryClause.AddRangeClause('S', IDimCoordinateQueryClause::RangeClause{ 10,10 });
    auto query = QueryBuildUtils::Build(
        internalDb->GetDb(),
        *docInfo,
        &queryClause,
        nullptr);
    auto sqlCommand = RemoveMultipleConsecutiveBlanks(query.getExpandedSQL());
    EXPECT_STRCASEEQ(sqlCommand.c_str(), "SELECT Pk FROM TILESINFO WHERE ((Dim_S= 10))");
}

TEST(QueryBuildUtilsTests, UseDimCoordinateQueryCheckSqlStatement4)
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
    queryClause.AddRangeClause('S', IDimCoordinateQueryClause::RangeClause{ 3,7 });
    auto query = QueryBuildUtils::Build(
        internalDb->GetDb(),
        *docInfo,
        &queryClause,
        nullptr);
    auto sqlCommand = RemoveMultipleConsecutiveBlanks(query.getExpandedSQL());
    EXPECT_STRCASEEQ(sqlCommand.c_str(), "SELECT Pk FROM TILESINFO WHERE ((Dim_S>= 3 AND Dim_S<= 7))");
}

TEST(QueryBuildUtilsTests, UseDimCoordinateQueryCheckSqlStatement5)
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
    queryClause.AddRangeClause('S', IDimCoordinateQueryClause::RangeClause{ numeric_limits<int>::min(),7 });
    auto query = QueryBuildUtils::Build(
        internalDb->GetDb(),
        *docInfo,
        &queryClause,
        nullptr);
    auto sqlCommand = RemoveMultipleConsecutiveBlanks(query.getExpandedSQL());
    EXPECT_STRCASEEQ(sqlCommand.c_str(), "SELECT Pk FROM TILESINFO WHERE ((Dim_S<= 7))");
}

TEST(QueryBuildUtilsTests, UseEmptyDimCoordinateQueryCheckSqlStatement3)
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
    auto query = QueryBuildUtils::Build(
        internalDb->GetDb(),
        *docInfo,
        &queryClause,
        nullptr);
    auto sqlCommand = RemoveMultipleConsecutiveBlanks(query.getExpandedSQL());
    EXPECT_STRCASEEQ(sqlCommand.c_str(), "SELECT Pk FROM TILESINFO");
}

TEST(QueryBuildUtilsTests, UseTileInfoQueryCheckSqlStatementEqual)
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

    TileInfoQueryClause tileInfoQueryClause(ConditionalOperator::Equal, 42);
    auto query = QueryBuildUtils::Build(
        internalDb->GetDb(),
        *docInfo,
        nullptr,
        &tileInfoQueryClause);
    auto sqlCommand = RemoveMultipleConsecutiveBlanks(query.getExpandedSQL());
    EXPECT_STRCASEEQ(sqlCommand.c_str(), "SELECT Pk FROM TILESINFO WHERE (PyramidLevel= 42)");
}

TEST(QueryBuildUtilsTests, UseTileInfoQueryCheckSqlStatementGreater)
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

    TileInfoQueryClause tileInfoQueryClause(ConditionalOperator::GreaterThan, 2);
    auto query = QueryBuildUtils::Build(
        internalDb->GetDb(),
        *docInfo,
        nullptr,
        &tileInfoQueryClause);
    auto sqlCommand = RemoveMultipleConsecutiveBlanks(query.getExpandedSQL());
    EXPECT_STRCASEEQ(sqlCommand.c_str(), "SELECT Pk FROM TILESINFO WHERE (PyramidLevel> 2)");
}

TEST(QueryBuildUtilsTests, UseDimCoordinateQueryAndTileInfoQueryCheckSqlStatement)
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
    queryClause.AddRangeClause('S', IDimCoordinateQueryClause::RangeClause{ 10,10 });
    TileInfoQueryClause tileInfoQueryClause(ConditionalOperator::LessThanOrEqual, 3);
    auto query = QueryBuildUtils::Build(
        internalDb->GetDb(),
        *docInfo,
        &queryClause,
        &tileInfoQueryClause);
    auto sqlCommand = RemoveMultipleConsecutiveBlanks(query.getExpandedSQL());
    EXPECT_STRCASEEQ(sqlCommand.c_str(), "SELECT Pk FROM TILESINFO WHERE ((Dim_S= 10)) AND (PyramidLevel<= 3)");
}