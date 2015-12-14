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
//
// Modification History:
// 07/15/2012 Created by Xu Lei
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Snt_Util_VirtualFile_h
#define Snt_Util_VirtualFile_h

#include "DataCacher/Ptrs.h"
#include "DataScanner/Ptrs.h"
#include "Porting/File.h"
#include "Porting/LongTypes.h"
#include "SEInterfaces/VirtualFileObj.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/FileDesc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/File.h"
#include "Util/KeyedNode.h"
#include "UtilComm/Ptrs.h"
#include "VirtualFile/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include <vector>

#define AOSREPLICATION_POLICY_INVALID			"invalid"
#define AOSREPLICATION_POLICY_NO_REP			"norep"
#define AOSREPLICATION_POLICY_TWO_COPIES		"two"
#define AOSREPLICATION_POLICY_THREE_COPIES		"three"
#define AOSSEGMENTTYPE_BYRECORDLEN				"segment_byrecordlen"
#define AOSSEGMENTTYPE_BYSEGMENT_SIZE			"segment_bysize"
#define AOSSEGMENTTYPE_BYLINEDELIMITRT  		"segment_bylinede"

class AosVirtualFile : public AosVirtualFileObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eDftSegmentSize		= 100000000,
		eMaxDelimiterLength = 100,
		eMinBlockSize		= 1000*1000,		  	//1M
		eMaxBlockSize		= 120 * 1000 * 1000		// 120 M
	};

	enum Operation 
	{
		eRead,
		eWrite
	};
	
	enum SegmentType
	{
		eInvalidSegmentType,

		eSegmentType_BySegmentSize,
		eSegmentType_ByRecordLen,
		eSegmentType_ByLineDelimiter,

		eMaxSegmentType
	};

	enum ReplicatePolicy
	{
		eInvalidRepliPolicy,

		eNoReplication,
		eReplicationPolicy_Two,
		eReplicationPolicy_Three,

		eErrorPolicy
	};

private:
	int					mRecordLen;
	u32 				mRetrieveLoopIdx;
	u64					mSegmentSize;
	bool				mIsGood;
	bool				mIsPublic;
	bool				mNeedOrder;
	bool				mNeedCompress;
	OmnMutexPtr			mLock;
	Operation			mOperation;
	AosBuffPtr			mCachedBuff;
	OmnString			mObjid;
	OmnString			mSeparator;
	OmnString 			mCompressType;
	SegmentType			mSegmentType;
	AosXmlTagPtr		mFileDoc;
	ReplicatePolicy		mReplicationPolicy;
	vector<u64>			mSegmentVect;	

public:
	AosVirtualFile(
			const OmnString &objid, 
			AosRundata *rdata);
	
	AosVirtualFile(
			const AosXmlTagPtr &def, 
			const AosXmlTagPtr &file_doc, 
			AosRundata *rdata);
	
	~AosVirtualFile();
	
	// Chen Ding, 10/30/2012
	virtual bool read(const AosBuffPtr &buff, const int64_t &len, AosRundata *rdata);

	static bool	fileExist(const OmnString &objid, AosRundata *rdata);
	u64			getFileLength(AosRundata *rdata) const;
	bool		close();
	vector<u64>& getAllSegment(AosRundata *rdata);
	bool		getFirstSegment(AosBuffPtr &buff, AosRundata *rdata);
	bool		nextSegment(AosBuffPtr &buff, bool &finished, AosRundata *rdata);
	bool		readSegment(
					const u64 &segment_id, 
					AosBuffPtr &buff, 
					AosRundata *rdata);
	bool		exportFile(u64 &file_len, const OmnString &fileName, AosRundata *rdata);
	bool		importFile(const OmnString &filename, AosRundata *rdata);
	bool 		appendData(const AosBuffPtr &buff, AosRundata *rdata);
	bool		createSegmentToAssignVir(
				const int &vid,	
				const AosBuffPtr &buff, 
				AosRundata *rdata);
	bool		deleteFromDisk(AosRundata *rdata);
	OmnString	getSegmentObjid(AosRundata *rdata)const;
	OmnString	getContainerObjid()const;
	SegmentType	getSegmentType()const{return mSegmentType;};
	u64 		getSegmentSize() const {return mSegmentSize;};
	bool 		serializeFrom(const AosBuffPtr &buff, AosRundata *rdata);
	bool 		serializeTo(const AosBuffPtr &buff, AosRundata *rdata);
	void		changeMode();	
	
	virtual bool append(const char *buff, const int len, const bool flush = false);
private:
	bool		init();
	bool		config(
					const AosXmlTagPtr &def, 
					const AosXmlTagPtr &file_doc,
					AosRundata *rdata);
	bool		retrieveReplicationPolicy(
				const AosXmlTagPtr &tag, 
				const AosXmlTagPtr &file_doc,
				AosRundata *rdata);
	bool		retrieveSegmentType(
				const AosXmlTagPtr &tag, 
				const AosXmlTagPtr &file_doc, 
				AosRundata *rdata);
	bool 		createEmptyFile(AosRundata *rdata);
	AosXmlTagPtr getFileDoc(AosRundata *rdata);
	bool		importFileByRecord(
				const int &record_len,
				const OmnFilePtr &file, 
				AosRundata *rdata);
	bool		importFileBySize(
				const u64 &segment_size,
				const OmnFilePtr &file, 
				AosRundata *rdata);
	bool		importFileByDelimiter(
				const OmnString &separator,
				const OmnFilePtr &file, 
				AosRundata *rdata);
	OmnString	getVFContainerName(const bool ispublic, AosRundata *rdata);
	OmnString	composeSegmentObjid(const u64 &seqno);	
	bool		createSegment(
				const int &vid,	
				const AosBuffPtr &buff, 
				AosRundata *rdata);
	bool        createNewFileDoc(     
				const AosXmlTagPtr &file_doc, 
				AosRundata *rdata);
	bool		createMemberContainer(AosRundata *rdata);
	bool		incrementFileSize(
				const u64 &size,
				AosRundata *rdata);
	bool		saveErrorLog(
				const OmnString &error, 
				const u64 &docid, 
				AosRundata *rdata);
	OmnString	SegmentTypeEnumToString(const SegmentType &type);
	u64			countLastSeparator(
				const AosBuffPtr &buff, 
				const OmnString &separator,
				AosRundata *rdata);

};
#endif

#endif
