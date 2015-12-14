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
// 2014/08/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StatServer_StatKeyConn_h
#define Aos_StatServer_StatKeyConn_h

#include "ReliableFile/Ptrs.h"
#include "Rundata/Ptrs.h"
//#include "SEUtil/FieldInfo.h"
#include "SEInterfaces/RlbFileType.h"
#include "Util/HashUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/BuffArrayVar.h"
#include "StatServer/Ptrs.h"

class AosVectorFile;
class AosAggrFuncObj;

class AosStatKeyConn : public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		//eDftRowsPerFile = 20000,
		eDftRowsPerFile = 500000,

		eMetaFileStart = 100,
		eKeyFileEntrySize = sizeof(u64)
	};

private:
	u64			mCubeId;
	u64			mMetaFileID;
	u32			mRowsPerFile;
	AosReliableFilePtr	mMetaFile;
	AosRlbFileType::E mFileType;
	
	AosBuffPtr 	mKeyFileIDsBuff;
	u64	*		mKeyFileIDsArray;
	
	u32			mNumKeyFiles;
	vector<AosStatKeyFilePtr> mKeyFiles;
	vector<AosStatKeyFile*>	mKeyFilesRaw;

	//yang
	OmnString mStatFunc;


	u64                 mReadTime1;
	u64                 mReadNum1;

	u64                 mReadTime2;
	u64                 mReadNum2;

	u64                 mReadTime3;
	u64                 mReadNum3;

	//arvin 2015.11.04
	//JIMODB-1075
	u64				    mTestPrevSdocid;
public:
	AosStatKeyConn(
			const u64 cube_id,
			const AosRlbFileType::E file_type);
	~AosStatKeyConn();

//yang
	void setStatFunc(OmnString statfunc)
	{
		mStatFunc=statfunc;
	}
	bool isDistCount()
	{
		return (mStatFunc=="dist_count") ? true : false;
	}

	bool 	config(
			const AosRundataPtr &rdata,
			const AosXmlTagPtr &conf);

	bool 	saveKey(
			AosRundata *rdata, 
			const u64 sdocid, 
			const OmnString &key);

	OmnString readKey(
			AosRundata *rdata, 
			const u64 sdocid); 

	bool 	readKeys(
			AosRundata *rdata, 
			const u64 sdocid, 
			const int num_records,
			AosBuffArrayVar *keys);

	void outputCounters();
	void initCounters();

private:
	bool 	readMetaFile(AosRundata *rdata);
	bool 	saveMetaFile(AosRundata *rdata);

	AosReliableFilePtr openFile(
			AosRundata *rdata, 
			const u64 file_id);

	AosStatKeyFile * getKeyFile(
			AosRundata *rdata, 
			const u64 sdocid);

};
#endif



