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
// 12/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeLogSvr_MdlLogSvr_h
#define AOS_SeLogSvr_MdlLogSvr_h

#include "Rundata/Ptrs.h"
#include "ReliableFile/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SeLogSvr/Ptrs.h" 
#include "SeLogSvr/SeLog.h"
#include "SeLogUtil/LogUtil.h"
#include "SeLogUtil/LogOpr.h"
#include "StorageMgr/FileKeys.h"
#include "StorageMgr/Ptrs.h"
#include "Util/CappedHash.h"
#include "UtilHash/StrObjHash.h"
#include "XmlUtil/XmlTag.h"
#include <map>
#include <ext/hash_map>
using namespace std;


#define AOSSITESEQNOPREFIX 						"site_prefix"
#define AOSLOGGLOBALIILNAME 					"log_iilname"


class AosMdlLogSvr : virtual public OmnRCObject 
{
	OmnDefineRCObject;

public:

	////////////////////////////////////////////
	// Just for test
	typedef hash_map<const OmnString, AosHashedObjPtr, Omn_Str_hash, compare_str> Map_t;
	typedef hash_map<const OmnString, AosHashedObjPtr, Omn_Str_hash, compare_str>::iterator MapItr_t;
	////////////////////////////////////////////
	enum
	{
		eMaxLogsPerContainer = 10,

		eActiveFlag = 'A',
		eDeletedFlag = 'D',
		eInvalidFlag = 'I',
		
		eInitialSeqno = 0,

		// for hash map
		eMaxMapSize = 100,
		eKeyBitMap = 20,
		eBucketSize = 1024,
	
		eLogMapFileSize = 1000000,
	};
	
private:
	OmnMutexPtr 			mLock;
	int						mMaxLogsPerContainer;
	AosStrObjHashPtr		mSeLogMap;
	AosStrObjHashPtr		mSeqnoMap;
	u32 					mModuleId;
public:
	AosMdlLogSvr(const int &mIdx);
	~AosMdlLogSvr();

	bool					stop();		// Ketty 2013/03/11

	//bool 					addLog(const AosXmlTagPtr &trans_data, const AosRundataPtr &rdata);
	bool 				addLog( 
							const AosLogOpr::E opr,
							const OmnString &pctr_objid,
							const OmnString logname,
							const AosXmlTagPtr &loginfo, 
							u64 &logid,
							const AosRundataPtr &rdata);

	int  					getMaxLogsPerContainer() 
							{
								return mMaxLogsPerContainer;
							}

	// Ketty 2012/12/05
	//OmnFilePtr 				getLogFile(
	AosReliableFilePtr		getLogFile(
							const u32 seqno,
							const u32 moduleId,
							const AosRundataPtr &rdata AosMemoryCheckDecl);

	AosXmlTagPtr			retrieveLog(
							const u64 &logid,
							const AosRundataPtr &rdata);
	
	bool 					saveSeLog(
							const AosSeLogPtr &selog, 
							const AosRundataPtr &rdata);

	// Ketty 2012/12/05
	//OmnFilePtr 				createNewLogFile(
	AosReliableFilePtr		createNewLogFile(
							u32 &seqno,
							const OmnString &pctr_objid,
							const OmnString &log_name,
							const u64 &moudleId,
							const AosRundataPtr &rdata AosMemoryCheckDecl);

	bool 					removeOldestFile(
							const OmnString &pctr_objid, 
							const OmnString &log_name,
							const u64 &moduleId,
							const AosRundataPtr &rdata);
	
	static inline OmnString	composeLogKey(
							const u32 siteid,
							const OmnString &pctnr_objid, 
							const OmnString &logname)
							{
								aos_assert_r(pctnr_objid != "", "");
								aos_assert_r(logname != "", "");
								aos_assert_r(siteid != 0, "");
								OmnString key;
								key << siteid << "_" << pctnr_objid << "_" << logname;
								return key;
							}

private:
	bool					init();
	bool 					removeLogFilePriv(
							const u32 seqno, 
							const AosRundataPtr &rdata);

	bool 					removeFromIIL(
							const u64 &fileSeqno,
							const AosRundataPtr &rdata);

	AosSeLogPtr 			createLog(
							const OmnString &pctnr_objid, 
							const OmnString &logname,
							const int &moduleId,
							const AosRundataPtr &rdata);
	
	inline OmnString 		composeSiteLastSeqnoKey(const u32 siteid)
							{
								aos_assert_r(siteid != 0, "");
								OmnString key = AOSSITESEQNOPREFIX;
								key << "_" << siteid;
								return key;
							}

	// Ketty 2012/11/29
	/*
	inline OmnString 		composeSelogHashFname(const int &moduleid)
							{
								OmnString fname = AOSFILEKEY_SELOGMGR_SELOG_HASH;
								fname << "_" << moduleid;
								return fname;
							}
	*/

	inline OmnString 		composeSeqnoKey(
							const u32 siteid,
							const u32 seqno)
							{
								aos_assert_r(siteid != 0, "");
								OmnString key;
								key << siteid <<  "_" << seqno; 
								return key;
							}

	// Ketty 2012/11/29
	/*
	inline OmnString 		composeSeqnoMapFname(const int &moduleid)
							{
								OmnString fname = AOSFILEKEY_SELOGMGR_SEQNO_HASH;
								fname << "_" << moduleid;
								return fname;
							}
	*/
	
	inline OmnString 		composeFilePrefixName(
							const u32 siteid,
							const u64 &seqno,
							const u32 &moduleId)
							{
								aos_assert_r(siteid != 0, "");
								aos_assert_r(seqno > 0, "");
								aos_assert_r(moduleId >= 0, "");
								OmnString key = "";
								key << moduleId
									<< "_" <<siteid
									<< "_" << seqno;
								return key;
							}
	
	void					showInformation();
private:
	// Ketty 2012/12/05
	//OmnFilePtr createLogFile(
	AosReliableFilePtr createLogFile(
							const u32 seqno,
							const u32 moduleId,
							u64 &file_seqno,
							const AosRundataPtr &rdata AosMemoryCheckDecl);
};
#endif
