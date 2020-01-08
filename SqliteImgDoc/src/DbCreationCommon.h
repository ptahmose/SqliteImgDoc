#pragma once
#include <string>
#include <sstream>
#include "miscutilities.h"

class CDbCreationCommon
{
protected:
    template <typename tAdap>
    std::string GetPerTileDataTableSqlStatementCommon(const tAdap& adap) const
    {
        const auto& coordinateDataColInfo = adap.GetPerTileColumnInfo();// this->docInfo.GetCoordinateDataColumnInfo();
        if (coordinateDataColInfo.empty())
        {
            return MiscUtils::empty_string;
        }

        auto ss = std::stringstream();
        ss << "CREATE TABLE[" << adap.GetTableNamePerTileData() /*this->docInfo.GetTableName(IDbDocInfo3D::TableType::PerBrickData)*/ << "](";
        ss << adap.GetColumnNamePk()/*this->docInfo.GetPerTilesDataColumnName(IDbDocInfo3D::PerTileDataColumn::Pk)*/ << " INTEGER";

        for (const auto& ci : /*this->docInfo.GetCoordinateDataColumnInfo()*/coordinateDataColInfo)
        {
            switch (ci.type)
            {
            case ColumnType::Integer:
                ss << ",[" << ci.columnName << "] INTEGER(" << ci.size << ")";
                break;
            case ColumnType::Float:
                ss << ",[" << ci.columnName << "] DOUBLE";
                break;
            }
        }

        ss << ",FOREIGN KEY(" << adap.GetColumnNamePk()/*this->docInfo.GetPerTilesDataColumnName(IDbDocInfo3D::PerTileDataColumn::Pk)*/ << ") REFERENCES " <<
            adap.GetTableNameTilesInfo()/*this->docInfo.GetTableName(IDbDocInfo3D::TableType::TilesInfo)*/ << "(" << 
            adap.GetColumnNamePkTilesInfo()/*this->docInfo.GetTileInfoColumnName(IDbDocInfo3D::TilesInfoColumn::Pk)*/ << ")";

        ss << ");";

        return ss.str();
    }
};
