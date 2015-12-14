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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 06/16/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SyBaseMgr_SyBaseMgr_h
#define AOS_SyBaseMgr_SyBaseMgr_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "SyBaseMgr/Ptrs.h"
#include "SyBaseMgr/Column.h"

#include <sybfront.h>
#include <sybdb.h>
#include <vector>
#include <map>

class AosSyBaseMgr : public OmnRCObject
{
	OmnDefineRCObject;
private:
	enum
	{
		eDefaultPort = 1433
	};

	struct Column
	{
		char* colName;
		char* colBuffer;
		int colType;
		int colSize;
		int colStatus;
	};

	bool 								mIsConnected;
	DBPROCESS*							mDbProcess;
	unsigned int 						mPort;
	OmnString							mHost;
	OmnString							mUserName;
	OmnString							mPassWord;
	OmnString							mDBName;

public:
	AosSyBaseMgr(
		const OmnString &hosts,
		const OmnString &username,
		const OmnString &passwd,
		const OmnString &dbname,
		const unsigned int port = eDefaultPort);

	~AosSyBaseMgr();

	bool initConnection();
	void setUserName(const OmnString &username);
	void setHost(const OmnString &host);
	void setPassword(const OmnString &password);
	void setDBname(const OmnString &dbname);
	void setPort(unsigned int port);

	int runSQLCommand(const OmnString &sqlstr);
	vector< vector<AosColumnPtr> > executeDataSet(const OmnString &sqlstr);

	void destroyConnection();
	bool getConnectionStatus();
};
#endif

