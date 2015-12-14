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
// 02/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SmartObj_TransMgrMgr_SobjTransMgr_h
#define Aos_SmartObj_TransMgrMgr_SobjTransMgr_h

#include "Rundata/Ptrs.h"
#include "SmartObj/TransMgr/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"

class SobjTransMgr : public virtual OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxTransFileLimit = 10000,
		eMaxTransFiles = 1000,
		eDftMaxFilesize = 10000000,
		eMaxFilesizeLimit = 100000000,
		eDftMaxTrans = 100000,
		eResponseEvent = 0,
		eMaxEventLimit = 30
	};

	enum TransModel
	{
		eInvalid,

		eMarkResponseOnly,
		eMarkAndSaveResponse,
		eUseEventConds,

		eMax
	};

	vector<SobjTransFilePtr>	mTransFiles;
	SobjTransFilePtr 			mCrtTransFile;
	AosXmlTagPtr				mDefiningDoc;
	AosXmlTagPtr				mEventConds;
	OmnMutexPtr					mLock;
	vector<bool>				mBitmaps;
	vector<bool>				mRespPos;
	int							mMaxTrans;
	int							mMaxFilesize;
	int							mMaxTransFile;
	TransModel					mTransModel;
	OmnString					mTransidAname;
	bool						mFlushFlag;
	u64							mVirtualDir;
	OmnString					mFilePrefix;
	u64							mTransidMapIILID;
	u32							mCrtSeqno;
	OmnString					mTransMgrName;

public:
	SobjTransMgr(const OmnString &name, const AosRundataPtr &rdata);
	SobjTransMgr(
		const AosXmlTagPtr &definingDoc, 
		const AosRundataPtr &rdata);
	~SobjTransMgr();

	bool addTrans(u32 &transid, const AosXmlTagPtr &trans, const AosRundataPtr &rdata);
	bool procResponse(const AosXmlTagPtr &response, const AosRundataPtr &rdata);
	bool logError(const OmnString &errmsg, const AosXmlTagPtr &msg);

	// Chen Ding, 03/02/2012
	bool addTrans(  const AosTrans1Ptr &trans,
					const u32 host_id,
					const u32 conn_id,
					const AosXmlTagPtr &trans_xml,
					const u32 client_moduleid,
					const u32 client_id);

	bool getTransHeaders(               
			        const u32 &start_local_id,
					AosTransHeader * headers,
					u32 &read_num);

	bool getTrans(const AosTransHeader &trans_header, OmnString &trans_str);
	bool readDoc(const u32 seqno, const u64 &offset, char *data, const u32 docsize);
	bool recover();
	bool clean(bool &finish);
	bool removeOldFiles();

private:
	bool init(const AosRundataPtr &rdata);
	SobjTransFilePtr getTransFile(const u32 seqno, const AosRundataPtr &rdata);
	bool transidToSeqno(u32 &seqno, const u32 transid, const AosRundataPtr &rdata);
	bool createNewFileLocked(const u32 transid, const AosRundataPtr &rdata);
	bool openCrtFileLocked(const AosRundataPtr &rdata);
	bool updateSeqnoToDbLocked(const AosRundataPtr &rdata);
	bool initWithDefiningDoc(const AosRundataPtr &rdata);

	bool addTransidSeqnoEntry(
						const u32 seqno, 
						const u32 transid, 
						const AosRundataPtr &rdata);

	int  getEventId( 	const AosXmlTagPtr &response, 
						bool &mark, 
						bool &track, 
						const AosRundataPtr &rdata);
};
#endif

