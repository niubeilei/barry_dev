////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/20/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeSysLog_SeSysLog_h
#define AOS_SeSysLog_SeSysLog_h

#include "SEUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Thread/Ptrs.h"
#include "aosUtil/Types.h"
#include "Util/File.h"
#include "XmlParser/Ptrs.h"
#include "XmlParser/XmlParser.h"
#include "TransUtil/Ptrs.h"
//#include "TransUtil/DsTransFileMgr.h"
#include "SearchEngine/Ptrs.h"
#include "Util/HashUtil.h"
#include "TransClient/Ptrs.h"
#include "TransUtil/RoundRobin.h"
#include "Rundata/Rundata.h"

using namespace std;

class AosSeSysLog : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:

private:

	u32 mMaxLogId;
	enum
	{
		eSeqnoStartAddr = 100,
		eMax = 90000,
		eStartFlag = 0xa4bfd315,
		eEndFlag = 0xa4bfd316,
		eSeqnoNums = 50,
		eStartFlagOffset = 0,
		eValueOffset = 4,
		eEndFlagOffset = 8,

		eTimeOut = 10
	};

	OmnMutexPtr		mLock;
	OmnFilePtr      mFile;
	OmnFilePtr      mSeqnoFile;
	u64				mFileSize;
	u64				mMaxFileSize;
	OmnString		mDirName;
	OmnString		mFileName;
	OmnString		mTransFileName;
	OmnString		mSeqnoFileName;
	char *          mBuffs;
	//static AosDsTransFileMgrPtr mTransFile;	// Ketty 2013/02/2
	u64				mGlobalLogId;
	AosTransClientPtr mTransClient;

public:
	AosSeSysLog();
	~AosSeSysLog();
	bool config(const AosXmlTagPtr &def);

	bool addCreateLog(
				const u64 userid, 
				const AosXmlTagPtr &doc, 
				const AosRundataPtr &rdata);

	bool addDeleteLog(
				const u64 &userid,
				const OmnString &appname,
				const AosXmlTagPtr &doc, 
				const AosRundataPtr &rdata);

	bool addModifyLog(
				const u64 userid,
				const AosXmlTagPtr &doc,
				const AosXmlTagPtr &orgdoc,
				const bool synobj, 
				const AosRundataPtr &rdata);

	bool addModifyAttrLog(
				const OmnString &aname,
				const OmnString &oldvalue,
				const OmnString &newvalue,
				const bool exist,
				const bool value_unique,
				const bool docid_unique,
				const u64 &docid, 
				const AosRundataPtr &rdata);

	bool addModifyAttrLog(
				const OmnString &aname,
				const u64 &oldvalue,
				const u64 &newvalue,
				const bool exist,
				const bool value_unique,
				const bool docid_unique,
				const u64 &docid, 
				const AosRundataPtr &rdata);
	
	bool addAddMemberLog(
				const AosXmlTagPtr &doc,
				const OmnString &ctnr_objid,
				const u64 &userid, 
				const AosRundataPtr &rdata);

	bool addRemoveMemberLog(
				const AosXmlTagPtr &doc,
				const OmnString &ctnr_objid, 
				const AosRundataPtr &rdata);
	
	bool addModifyObjRequest(
				const AosXmlTagPtr &root, 
				const AosXmlTagPtr &log, 
				const AosRundataPtr &rdata);

	bool addCreateRequest(
				const AosXmlTagPtr &root, 
				const AosXmlTagPtr &log, 
				const AosRundataPtr &rdata);

	bool addModifyAttrRequest_str(
				const AosXmlTagPtr &root, 
				const AosXmlTagPtr &log, 
				const AosRundataPtr &rdata);

	bool addModifyAttrRequest_u64(
				const AosXmlTagPtr &root, 
				const AosXmlTagPtr &log, 
				const AosRundataPtr &rdata);

	bool addDeleteObjRequest(
				const AosXmlTagPtr &root, 
				const AosXmlTagPtr &log, 
				const AosRundataPtr &rdata);

	bool recoverSystem(
				const AosXmlTagPtr &root, 
				const AosXmlTagPtr &syncData, 
				const AosRundataPtr &rdata);

	//static AosDsTransFileMgrPtr getTransFile()
	//{
	//	return mTransFile;
	//}

private:
	OmnFilePtr 		getFile(bool readOnly = false);
	OmnFilePtr 		readFile(u32 seqno);
	bool			openSeqnoFile();
	u32				getSeqno();
	u32				getLogId();
	bool			setSeqno(u32 value);
	bool			intToBool(int value);
};
#endif

