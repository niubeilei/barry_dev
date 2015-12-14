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
//
// Modification History:
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtService_H
#define AOS_JQLStatement_JqlStmtService_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtService : public AosJqlStatement
{
public:
	enum ServiceStatus 
	{
		eInvalid = 0,
		
		eRunning,
		eStopped,
		eFailed,
		eStarted,

		eMax
	};

private:
	//data from JQLParser
	OmnString			mServiceName;
	//vector<AosExprNameValuePtr> mConfParms;
	OmnString			mJobName;
	ServiceStatus		mStatus;
	u32					mMaxThread;
	u32					mMaxTask;
	static OmnString	mSysDB;  // = "_sysdb";
	static OmnString	mSysTable; //= "_service";

public:
	AosJqlStmtService();
	~AosJqlStmtService();

	//getter/setters
	void setServiceName(OmnString name) { mServiceName = name; }
	//void setConfParms(vector<AosExprNameValuePtr> parms) {mConfParms = parms; }
	void setJobName(OmnString name) { mJobName = name; }
	void setMaxThread(u32 num){ mMaxThread = num;}
	void setTaskNum(u32 num){ mMaxTask = num;}

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	bool createService(const AosRundataPtr &rdata);  
	//bool runService(const AosRundataPtr &rdata);  
	bool startService(const AosRundataPtr &rdata);  
	bool stopService(const AosRundataPtr &rdata);  
	bool showServices(const AosRundataPtr &rdata);   
	bool describeService(const AosRundataPtr &rdata);
	bool dropService(const AosRundataPtr &rdata);   

	AosXmlTagPtr convertToXml(const AosRundataPtr &rdata);

	static bool createServiceSysTable();
	static bool addServiceData(OmnString serviceName, AosXmlTagPtr data);

private:

};

#endif
