////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This is a utility to select docs.
//
// Modification History:
// 06/16/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "SyBaseMgr/SyBaseMgr.h"
#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"

#include <iomanip>

AosSyBaseMgr::AosSyBaseMgr(
		const OmnString &host,
		const OmnString &username,
		const OmnString &passwd,
		const OmnString &dbname,
		const unsigned int port)
:
mIsConnected(false),
mPort(port),
mHost(host),
mUserName(username),
mPassWord(passwd),
mDBName(dbname)
{
}


AosSyBaseMgr::~AosSyBaseMgr()
{
	if (mIsConnected)
	{
		destroyConnection();
	}
}

bool
AosSyBaseMgr::initConnection()
{
	OmnString charset = "UTF-8";
	dbinit();
	LOGINREC* loginrec = dblogin();
	DBSETLUSER(loginrec, mUserName.data());
	DBSETLPWD(loginrec, mPassWord.data());
	DBSETLCHARSET(loginrec, charset.data());
	OmnString server;
	server << mHost << ":" << mPort;
	mDbProcess = dbopen(loginrec, server.data());
	if (mDbProcess == FAIL)
	{
		OmnAlarm << "Connect to SQL Server failed!. Host Address: " <<
			server << enderr;
		return false;
	}

	if (dbuse(mDbProcess, mDBName.data()) == FAIL)
	{
		OmnAlarm << "Use DataBase failed!. DataBase Name: " <<
			mDBName << enderr;
		return false;
	}
	mIsConnected = true;
	return true;
}

vector< vector<AosColumnPtr> >
AosSyBaseMgr::executeDataSet(const OmnString &sqlstr)
{
	vector< vector<AosColumnPtr> > dataset;
	dataset.clear();
	aos_assert_r(mDbProcess, dataset);
	dbcmd(mDbProcess, sqlstr.data());
	if (dbsqlexec(mDbProcess) == FAIL)
	{
		OmnAlarm << "Query error. Query string: " << 
			sqlstr << enderr;
		return dataset;
	}
	DBINT result_code;
	while((result_code = dbresults(mDbProcess)) != NO_MORE_RESULTS)
	{
		u32 num_cols;
		int rowNo;
		if (result_code == SUCCEED)
		{
			vector<AosColumnPtr> columns;
			num_cols = dbnumcols(mDbProcess);
			for (u32 i=0; i<num_cols; i++)
			{
				AosColumnPtr pCol = OmnNew AosColumn();
				int colNo = i+1;
				pCol->setColName(dbcolname(mDbProcess, colNo));
				pCol->setColType(dbcoltype(mDbProcess, colNo));
				pCol->setColSize(dbcollen(mDbProcess, colNo));

				if (SYBCHAR != pCol->getColType())
				{
					pCol->setColSize(dbwillconvert(pCol->getColType(), SYBCHAR));
				}
				(pCol->getColBufferRef()).setLength(pCol->getColSize()+1);
				if (dbbind(mDbProcess, colNo, STRINGBIND, (DBINT)(pCol->getColSize() + 1), (BYTE*)(pCol->getColBufferRef()).getBuffer()) == FAIL)
				{
					OmnAlarm << "Running dbind() error!" << enderr;
					return dataset;
				}

				if (dbnullbind(mDbProcess, colNo, &pCol->getColStatusRef()) == FAIL)
				{
					OmnAlarm << "Running dbnullbind() error!" << enderr;
					return dataset;
				}
				columns.push_back(pCol);
			}
			while((rowNo = dbnextrow(mDbProcess)) != NO_MORE_ROWS)
			{
				switch(rowNo)
				{
				case REG_ROW:
				{
					vector<AosColumnPtr> cols;
					for (u32 i=0; i<columns.size(); i++)
					{
						AosColumnPtr col = OmnNew AosColumn();
						if (columns[i]->getColStatus() == -1)
						{
							col->setColBuffer("");
						}
						else
						{
							col->setColBuffer(columns[i]->getColBuffer());
						}
						col->setColSize(columns[i]->getColSize());
						col->setColName(columns[i]->getColName());
						col->setColType(columns[i]->getColType());
						col->setColStatus(columns[i]->getColStatus());
						cols.push_back(col);
					}
					dataset.push_back(cols);
				}
				break;
				case BUF_FULL:
				aos_assert_r(rowNo != BUF_FULL, dataset);
				break;
				default:
					OmnAlarm << "Get result ignore, row number: " << rowNo << enderr;
				}
			}
			if (DBCOUNT(mDbProcess) > -1)
			{
				OmnScreen << "Affected rows: " << DBCOUNT(mDbProcess) << endl;
			}
			if (dbhasretstat(mDbProcess) == TRUE)
			{
				OmnScreen << "Procedure returned" << dbhasretstat(mDbProcess) << endl;
			}
		}
	}
	return dataset;
}


int 
AosSyBaseMgr::runSQLCommand(const OmnString &sqlstr)
{
	aos_assert_r(mDbProcess, 0);
	dbcmd(mDbProcess, sqlstr.data());
	if (dbsqlexec(mDbProcess) == FAIL)
	{
		OmnAlarm << "Query error. Query string: " << 
			sqlstr << enderr;
		return 0;
	}
	int count = DBCOUNT(mDbProcess);
	if (count > -1)
	{
		OmnScreen << "Affected rows: " << count << endl;
	}
	return count;
}

void
AosSyBaseMgr::destroyConnection()
{
	dbclose(mDbProcess);
}

bool
AosSyBaseMgr::getConnectionStatus()
{
	return mIsConnected;
}

void
AosSyBaseMgr::setHost(const OmnString &host)
{
	mHost = host;
}

void
AosSyBaseMgr::setPort(const unsigned int port)
{
	mPort = port;
}

void
AosSyBaseMgr::setUserName(const OmnString &username)
{
	mUserName = username;
}

void
AosSyBaseMgr::setPassword(const OmnString &password)
{
	mPassWord = password;
}

void
AosSyBaseMgr::setDBname(const OmnString &dbname)
{
	mDBName = dbname;
}

