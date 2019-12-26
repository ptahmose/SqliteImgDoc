#pragma once

#include "DbBase.h"

class CDbWriteBase : public CDbBase, public virtual SlImgDoc::IDbWriteTransaction
{
private:
    bool transactionPending;

public:
    CDbWriteBase(std::shared_ptr<CDb> db) : CDbBase(db), transactionPending(false) {}

    virtual void BeginTransaction();
    virtual void CommitTransaction();
    virtual void RollbackTransaction();
    virtual ~CDbWriteBase();
};
