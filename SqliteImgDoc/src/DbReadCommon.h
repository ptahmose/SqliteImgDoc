#pragma once
#include <iostream>
#include "../external/Interface.h"
#include "DbBase.h"
#include "miscutilities.h"
#include "dbutils.h"

class CDbReadCommon : public CDbBase
{
protected:
    CDbReadCommon(std::shared_ptr<CDb> db) :CDbBase(std::move(db)) {}

    template <typename t>
    void ReadPerTileDataCommon(const t& docAdapter, SlImgDoc::dbIndex idx, const std::vector<std::string>& columns, std::function<bool(const SlImgDoc::KeyVariadicValuePair&)> func)
    {
        std::stringstream ss;
        ss << "SELECT ";
        MiscUtils::AddCommaSeparatedAndEnclose(ss, columns.cbegin(), columns.cend(), "[", "]");
        ss << " FROM [" << docAdapter.GetTableName() << "] " <<
            "WHERE [" << docAdapter.GetPkColumnName() << "]=?1";
        SQLite::Statement query(this->GetDb(), ss.str());
        query.bind(1, idx);

        const auto& perTileDataColumnInfo = docAdapter.GetPerTileDataColumnInfo();
        try
        {
            while (query.executeStep())
            {
                const int colCnt = query.getColumnCount();
                for (int i = 0; i < colCnt; ++i)
                {
                    const auto& colName = query.getColumnName(i);
                    const auto& it = find_if(perTileDataColumnInfo.cbegin(), perTileDataColumnInfo.cend(), [&](const ColumnTypeAllInfo& info)->bool {return info.columnName == colName; });

                    auto kv = MiscUtils::ReadValue(query, i, *it);
                    if (!func(kv))
                    {
                        break;
                    }
                }
            }
        }
        catch (SQLite::Exception & excp)
        {
            std::cout << excp.what();
        }
    }

    template<typename ForwardIterator>
    void EnumPerTilesColumns(ForwardIterator begin, ForwardIterator end, const std::function<bool(const SlImgDoc::ColumnDescription&)>& func)
    {
        for (ForwardIterator it = begin; it != end; ++it)
        {
            SlImgDoc::ColumnDescription cd;
            cd.Name = it->columnName;
            cd.DataType = DbUtils::ColumnTypeInfoToStringRepresentation(*it);
            bool b = func(cd);
            if (!b)
            {
                break;
            }
        }
    }
};
