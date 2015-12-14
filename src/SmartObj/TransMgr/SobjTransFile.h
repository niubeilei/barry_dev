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
#ifndef Aos_SmartObj_TransMgr_TransFile_h
#define Aos_SmartObj_TransMgr_TransFile_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SmartObj/TransMgr/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/AppendFile.h"
#include "XmlUtil/XmlTag.h"

class SobjTransFile : public virtual OmnRCObject
{
	OmnDefineRCObject;

private:
	enum
	{
		eFileHeaderSize = 28,
		eRecordHeaderSize = 15,

		eFileHeaderOffset = 10,
		eTransPosOffset = 200,

		eMaxTransLimit = 1000000,
		eMaxEvents = 50,
		ePoison = 0x63fa4d,
		eMarkBitmask = 0x01,
		eTrackBitmask = 0x02,
		eMaxFileSize = 30000000		// 30M
	};

	OmnMutexPtr			mLock;
	bool				mIsGood;
	u32					mStartTransid;
	int					mMaxTrans;
	u32					mLastSavedPos;
	u32					mLastModTime;
	int					mMaxBitmaps;
	int					mMaxPositions;
	OmnString			mFname;
	OmnString			mTransAname;
	int					mFileMaxSize;
	bool				mFlushFlag;
	u32					mSeqno;
	u32					mNextTransId;
	u32		   		   *mTransPos;
	u32		   		   *mRespPos[eMaxEvents];
	u32		   		   *mBitmaps[eMaxEvents];
	AosAppendFilePtr 	mAppendFile;
	OmnFilePtr 			mReadFile;

public:
	SobjTransFile(
				  const OmnString &fname, 
				  const u32 seqno, 
				  const u32 start_transid,
				  const int maxTrans, 
				  const int maxFilesize, 
				  const OmnString &trans_aname,
				  const bool flush_flag,
				  const vector<bool> &bitmaps, 
				  const vector<bool> &respPos, 
				  const AosRundataPtr &rdata);

	SobjTransFile(
				  const OmnString &fname, 
		  		  const OmnString &trans_aname,
				  const bool flush_flag,
				  const vector<bool> &bitmaps, 
				  const vector<bool> &respPos, 
				  const AosRundataPtr &rdata);

	~SobjTransFile();

	bool addTrans( 	u32 &transid, 
					const AosXmlTagPtr &trans, 
					bool &needCreateNewFile,
					const AosRundataPtr &rdata);

	bool procResponse( 
					const u32 transid,
					const AosXmlTagPtr &response, 
					const int event_id,
					const bool mark,
					const bool track,
					const SobjTransMgrPtr &mgr,
					const AosRundataPtr &rdata);

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
	inline u32 getBitmapNumBytes() const
	{
		return mMaxTrans >> 3;
	}

	inline u32 getBitmapEntries() const
	{
		return mMaxTrans >> 5;
	}

	inline int getBitmapEntryIdx(const u32 transid, int &offset)
	{
		aos_assert_r(transid >= mStartTransid, -1);
		int idx = transid - mStartTransid;
		offset = idx & 0x1f;
		return (idx >> 5);
	}

	bool markBitmapLocked(
					const int event_id, 
					const u32 transid, 
					const AosXmlTagPtr &response,
					const SobjTransMgrPtr &mgr,
					const AosRundataPtr &rdata);

	bool saveResponseLocked(
					const int event_id,
					const u32 transid,
					const bool mark,
					const bool track,
					const AosXmlTagPtr &response, 
					const SobjTransMgrPtr mgr,
					const AosRundataPtr &rdata);

	int64_t appendRecord(
					const u32 transid,
					const int event_id,
					const bool mark,
					const bool track,
					const AosXmlTagPtr &trans, 
					const AosRundataPtr &rdata);

	int64_t appendRecord(
					const u32 transid,
					const int event_id,
					const bool mark,
					const bool track,
					const AosRundataPtr &rdata);

	bool readOneRecord(
					u32 &pos, 
					u32 &transid, 
					int &event_id, 
					bool &mark, 
					bool &track, 
					bool &finished, 
					const AosRundataPtr &rdata);

	bool updateRespPosLocked(
					const int event_id,
					const u32 transid,
					const u32 pos,
					const AosRundataPtr &rdata);

	bool createNewFile(
					const vector<bool> &bitmaps,
					const vector<bool> &resp_pos,
					const AosRundataPtr &rdata);

	bool openFileOnCreation(
					const vector<bool> &bitmaps,
					const vector<bool> &resp_pos,
					const AosRundataPtr &rdata);

	bool initFileLocked(const AosRundataPtr &rdata);
	bool readFromFile(const AosRundataPtr &rdata);
	bool readHeaderLocked(const AosRundataPtr &rdata);
	bool readTransPosLocked(u32 &pos, const AosRundataPtr &rdata);
	bool readRespPositionsLocked(u32 &pos, const AosRundataPtr &rdata);
	bool readBitmapsLocked(u32 &pos, const AosRundataPtr &rdata);
	bool rebuildControlInfoLocked(const AosRundataPtr &rdata);
	bool saveControlInfoLocked(const AosRundataPtr &rdata);
	bool saveHeaderLocked(const AosRundataPtr &rdata);
	bool saveBitmapsLocked(u32 &pos, const AosRundataPtr &rdata);
	bool saveRespPosLocked(u32 &pos, const AosRundataPtr &rdata);
	bool saveTransPosLocked(u32 &pos, const AosRundataPtr &rdata);
	bool openFileLocked(const AosRundataPtr &rdata);
	u32 getCrtTransid(const AosRundataPtr &rdata);
	bool closeFile(const AosRundataPtr &rdata);
	bool isFileTooBig();
	u32 getNextTransid();
};
#endif

