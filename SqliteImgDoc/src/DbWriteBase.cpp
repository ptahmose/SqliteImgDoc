#include "pch.h"
#include "DbWriteBase.h"
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>

#include <iostream>
#include <sstream>
#include <memory>

using namespace std;

/*virtual*/CDbWriteBase::~CDbWriteBase()
{
    if (this->transactionPending == true)
    {
        this->RollbackTransaction();
    }
}

/*virtual*/void CDbWriteBase::BeginTransaction()
{
    if (this->transactionPending != false)
    {
        throw runtime_error("A transaction was already pending.");
    }

    this->GetDb().exec("BEGIN");
    this->transactionPending = true;
}

/*virtual*/void CDbWriteBase::CommitTransaction()
{
    if (this->transactionPending != true)
    {
        throw runtime_error("There is no pending transaction.");
    }

    this->GetDb().exec("COMMIT");
    this->transactionPending = false;
}

/*virtual*/void CDbWriteBase::RollbackTransaction()
{
    if (this->transactionPending != true)
    {
        throw runtime_error("There is no pending transaction.");
    }

    try
    {
        this->GetDb().exec("ROLLBACK");
    }
    catch (SQLite::Exception&)
    {
        // TODO: deal with error, see https://www.sqlite.org/lang_transaction.html for the rules...
    }

    this->transactionPending = false;
}