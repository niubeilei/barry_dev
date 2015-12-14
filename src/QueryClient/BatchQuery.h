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
//	03/29/2013	Created by Jackie 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_QueryClient_BatchQuery_h
#define Omn_QueryClient_BatchQuery_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "aosUtil/Types.h"
#include "Debug/Debug.h"
#include "QueryUtil/QrUtil.h"
#include "Util/String.h"
#include "Util/MemoryChecker.h"
#include "Util/MemoryCheckerObj.h"
#include "Util/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Util/OmnNew.h"
#include "UtilComm/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/QueryRsltObj.h"
#include "SEInterfaces/DocClientCaller.h" 

#include <map>
#include <deque>
#include <queue>
#include <list>

struct AosBatchRecord 
{
	AosBatchRecord()
	:
	r_buff(NULL),
	r_buff_start(0),
	r_buff_len(0),
	r_isMarked(false)
	{
	};
	AosBuffPtr	r_buff;
	int			r_buff_start;
	int			r_buff_len;
	bool		r_isMarked;
};

class AosBatchQuery : virtual public OmnRCObject, public AosMemoryCheckerObj
{
	OmnDefineRCObject;
	enum
	{
		eMaxBlockSize = 10000,
		eShufferSize =  1000000
	};

	struct config
	{
		AosXmlTagPtr	format;

	};


private:

	int					mBuffSize;
	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;

	struct config		mConfig;
	OmnString			mQueryId;
	map<u64, struct AosBatchRecord> mDocidMap; //reset, docid map record
	u32					mProcDocsNum;
	bool				mFinished;// save all docs to database 
	AosQueryRsltObjPtr	mQueryRslt;
	map<OmnString, AosBuffPtr>	mColumns;
	vector<AosQrUtil::FieldDef>	mFieldDef;
	AosRundataPtr		mRundata;

	bool	reset(const AosRundataPtr &rdata);
	bool createColumns(const AosRundataPtr &rdata);

public:
	AosBatchQuery(const OmnString &queryid, 
				const AosXmlTagPtr &config, 
				const AosRundataPtr &rdata AosMemoryCheckDecl);
	~AosBatchQuery();
	bool	addQueryRslt(
					const AosRundataPtr &rdata, 
					const AosBuffPtr &fields,
					vector<AosQrUtil::FieldDef> fielddef,
					const AosQueryRsltObjPtr &queryrslt);
	bool	procDocs(const AosBuffPtr &buff, const AosRundataPtr &rdata);
	bool	isFinished(){return mFinished;}
	OmnString	getQueryId(){return mQueryId;}
	int	getBuffSize(){return mBuffSize;}
	map<OmnString, AosBuffPtr>	getData(){return mColumns;}

};
#endif




