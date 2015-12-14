////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 04/28/2013 Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_BuffData_h
#define Aos_SEInterfaces_BuffData_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/NetFileObj.h"
#include "SEInterfaces/SchemaObj.h"
#include "Util/DiskStat.h"
#include "Util/Buff.h"
#include "Util/MemoryChecker.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "MetaData/MetaData.h"
#include "MetaData/Ptrs.h"

#include <vector>

class AosBuffData : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum MetaDataType
	{
		eMetaDataType_Invalid,

		eMetaDataType_Docid, 
		eMetaDataType_Xml,

		eMetaDataType_Max
	};

	enum
	{
		eMaxMetadata = 100
	};

private:
	AosFileInfo				mFileInfo;
	AosBuffPtr				mBuff;
	int						mCrtBlockIdx;
	vector<AosXmlTagPtr>	mVectorDocs;		// Andy zhang 
	vector<AosBuffPtr>		mVectorBuffs;	// Andy zhang
	OmnString				mDataType;
	OmnString				mDataName;
	//vector<AosXmlTagPtr>	mMetadata;
	AosMetaDataPtr			mMetadata;
	AosXmlTagPtr			mHadoopMetadata;
	AosDiskStat				mDiskStat;
	AosSchemaObjPtr			mSchema;
	u64						mOffset;
	bool					mDiskError;
	bool					mIsEOF;
	int						mReqId;

	u64						mDocid;

public:
	AosBuffData()
	:
	mCrtBlockIdx(-1),
	mOffset(0),
	mDiskError(false),
	mIsEOF(false),
	mReqId(-1),
	mDocid(0)
	{
		mCrtBlockIdx = 0;
		mBuff = OmnNew AosBuff(100 AosMemoryCheckerArgs);	// Young, 2013/12/25
	}

	AosBuffData(
			const AosBuffPtr &buff, 
			const AosMetaDataPtr &metadata, 
			const AosSchemaObjPtr &schema)
	:
	mBuff(buff),
	mSchema(schema),
	mDiskError(false),
	mIsEOF(false),
	mReqId(-1),
	mDocid(0)
	{
		mMetadata = metadata;
	}

	AosBuffData(
			const AosMetaDataPtr &metadata, 
			const AosBuffPtr &buff, 
			const AosDiskStat &disk_stat)
	:
	mCrtBlockIdx(-1),
	mOffset(0),
	mDiskError(false),
	mIsEOF(false),
	mReqId(-1),
	mDocid(0)
	{
		mMetadata=metadata;
		mBuff = buff;
		mDiskStat = disk_stat;
	}

	~AosBuffData()
	{
	}

	void setDiskError(const bool disk_error){ mDiskError = disk_error; }
	bool getDiskError() const { return mDiskError; }

	void setEOF()
	{
		mIsEOF = true;
	}
	bool isEOF(){return mIsEOF;}

	void setFileInfo(const AosFileInfo &info) { mFileInfo = info; }
	AosFileInfo getFileInfo() const { return mFileInfo; }

	void setReqId(const int reqid){mReqId = reqid;}
	int getReqId(){return mReqId;}

	void setBuff(const AosBuffPtr &buff) { mBuff = buff; }
	AosBuffPtr getBuff() const { return mBuff; }

	void setCrtBlockIdx(int idx) { mCrtBlockIdx = idx; }
	int getCrtBlockIdx() { return mCrtBlockIdx; }

	// void setXmlDoc(const AosXmlTagPtr &doc) {mXmlDoc = doc;}

	// AosXmlTagPtr getXmlDoc() const {return mXmlDoc;}
	AosMetaDataPtr getMetadata() const
	{
		return mMetadata;
	}

	void setMetadata(AosMetaDataPtr &metadata)
	{
		mMetadata = metadata;
	}

	void setVectorDocs(const vector<AosXmlTagPtr> &docs) {mVectorDocs = docs; }
	vector<AosXmlTagPtr> getVectorDocs() {return mVectorDocs; }
	
	void setVectorBuffs(const vector<AosBuffPtr> &buffs){ mVectorBuffs = buffs;}
	vector<AosBuffPtr> getVectorBuffs(){ return mVectorBuffs;};

	void setDataType(const OmnString &data_type){mDataType = data_type;}
	OmnString getDataType(){return mDataType;}

	void setDataName(const OmnString &data_name){mDataName = data_name;}
	OmnString getDataName(){return mDataName;}

	void setDocid(const u64 docid) {mDocid = docid;}
	u64 getDocid() {return mDocid;}

	int64_t dataLen()
	{
		aos_assert_r(mBuff, -1);
		return mBuff->dataLen();
	}

	bool hasMore()
	{
		aos_assert_r(mBuff, false);
		return mBuff->hasMore();
	}

	bool appendBuff(const AosBuffPtr &buff)
	{
		aos_assert_r(mBuff, false);
		return mBuff->appendBuff(buff);
	}

	bool setCrtIdx(const int64_t &value)
	{
		aos_assert_r(mBuff, false);
		return mBuff->setCrtIdx(value);
	}

	int64_t getCrtIdx()
	{
		aos_assert_r(mBuff, -1);
		return mBuff->getCrtIdx();
	}

	char * data()
	{
		aos_assert_r(mBuff, 0);
		return mBuff->data();
	}

	void setDataLen(const int64_t len)
	{
		aos_assert(mBuff);
		mBuff->setDataLen(len);
	}

	void setDiskStat(const AosDiskStat &ds)
	{
		mDiskStat = ds;
	}
	int64_t getDataLen() const
	{
		if (!mBuff) return 0;
		return mBuff->dataLen();
	}

	void setOffset(const u64 offset) {mOffset = offset;}
	u64 getOffset() const {return mOffset;}
	
	AosBuffPtr buffData(){ return mBuff; };		// Ketty 2014/05/07

	static MetaDataType MetaDataType_Str2Enum(const OmnString type)
	{
		if (type == "zky_docid")
		{
			return eMetaDataType_Docid;
		}
		else if (type == "xml")
		{
			return eMetaDataType_Xml;
		}
		return eMetaDataType_Invalid;
	}

	static bool MetaDataType_IsValid(const MetaDataType type)
	{
		return (type > eMetaDataType_Invalid && type < eMetaDataType_Max);	
	}

};

#endif

