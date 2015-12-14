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
#ifndef Aos_SEInterfaces_BuffMetaData_h
#define Aos_SEInterfaces_BuffMetaData_h

#include "Rundata/Ptrs.h"
#include "Util/Buff.h"
#include "Util/MemoryChecker.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <vector>

class AosBuffMetaData : virtual public OmnRCObject
{
	OmnDefineRCObject;
private:
	AosFileInfo			mFileInfo;
	AosBuffPtr				mBuff;
	int						mCrtBlockIdx;
	vector<AosXmlTagPtr>	mVectorDocs;		// Andy zhang 
	vector<AosBuffPtr>		mVectorBuffs;	// Andy zhang
	AosXmlTagPtr			mXmlDoc;

	// JACKIE-HADOOP
	int64_t					mOffset;

public:
	AosBuffMetaData()
	{
		mCrtBlockIdx = 0;
		mVectorBuffs.clear();
		mVectorDocs.clear();
	}
	~AosBuffMetaData()
	{
		mVectorBuffs.clear();
		mVectorDocs.clear();
	}
	void setFileInfo(const AosFileInfo &info) { mFileInfo = info; }
	AosFileInfo getFileInfo() const { return mFileInfo; }

	void setBuff(const AosBuffPtr &buff) { mBuff = buff; }
	AosBuffPtr getBuff() const { return mBuff; }

	void setCrtBlockIdx(int idx) { mCrtBlockIdx = idx; }
	int getCrtBlockIdx() { return mCrtBlockIdx; }

	void setXmlDoc(const AosXmlTagPtr &doc) {mXmlDoc = doc;}
	AosXmlTagPtr getXmlDoc() const {return mXmlDoc;}

	void setVectorDocs(const vector<AosXmlTagPtr> &docs) {mVectorDocs = docs; }
	vector<AosXmlTagPtr> getVectorDocs() {return mVectorDocs; }
	
	void setVectorBuffs(const vector<AosBuffPtr> &buffs){ mVectorBuffs = buffs;}
	vector<AosBuffPtr> getVectorBuffs(){ return mVectorBuffs;};
	
	// JACKIE-HADOOP
	void setOffset(const int64_t & offset){mOffset = offset;}
	int64_t getOffset(){return mOffset;}

};
#endif

