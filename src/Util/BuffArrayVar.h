////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 2014/09/15 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_BuffArrayVar_h
#define AOS_Util_BuffArrayVar_h

#include "DataTypes/DataColOpr.h"
#include "Rundata/Ptrs.h"
#include "SEUtil/DocTags.h"
#include "Thread/Ptrs.h"
#include "Util/Buff.h"
#include "Util/CompareFun.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/ValueRslt.h"


class AosBuffArrayVar : public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum
	{
		eHeadSize = sizeof(int) + sizeof(i64),
		eInitHeadSize = 10000 * (sizeof(int) + sizeof(i64)),
		eInitBodySize = 10000 * 100,
		eMaxWasteSize = 10000,
		eMaxBodyBuffSize = 100000000, //100M
		eMaxBuffSize = 1000000000	// Ken Lee
	};

private:
	OmnMutexPtr			mLock;
	OmnMutex *			mLockRaw;
	AosCompareFunPtr 	mComp;
	AosFunCustom *		mCompRaw;
	AosBuffPtr			mHeadBuff;
	AosBuff *			mHeadBuffRaw;
	AosBuffPtr			mBodyBuff;
	AosBuff *			mBodyBuffRaw;
	i64					mBodyAddr;

	bool				mStable;
	bool				mIsSorted;
	i64					mNumRcds;
	i64					mWasteSize;
	int					mRemainSize;
	int					mHeadSize;

public:
	AosBuffArrayVar(const bool stable);
	AosBuffArrayVar(const AosBuffPtr &buff,const AosCompareFunPtr &comp, const bool stable);
	~AosBuffArrayVar();

private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
	
	i64		calcHeadNum(const i64 &crt_num);

	bool	rebuildData();
	bool	rebuildBodyAddr();

	i64		findPosPriv(
				const i64 &idx,
				const char *data,
				const int len,
				bool &unique);

	bool	insertEntryPriv(
				const i64 &idx,
				const char *data,
				const int len,
				AosRundata *rdata);

	bool buildNewHeaderBuff(
		const int rcd_offset,
		const i64 new_body_addr,
		char *body);

public:
	static AosBuffArrayVarPtr create(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);

	static AosBuffArrayVarPtr create(
				const AosCompareFunPtr &comp,
				const bool stable,
				const AosRundataPtr &rdata);
	static AosBuffArrayVarPtr create(
				const AosXmlTagPtr &cmp_tag,
				const bool stable,
				const AosRundataPtr &rdata);
	static bool	remainingBodyBuff(
				const AosBuffPtr &buff,
				int &remain_size);

	AosCompareFunPtr getCompareFunc() const {return mComp;} 
	void	setCompareFunc(const AosCompareFunPtr &func) 
	{
		mComp = func;
		mCompRaw = dynamic_cast<AosFunCustom*>(mComp.getPtr());
	}

	bool	isStable() const {return mStable;}
	bool	isSorted() const {return mIsSorted;}
	void	setSorted(const bool sorted) {mIsSorted = sorted;}

	bool	sort();
	bool	reset();
	
	i64		getNumEntries() const {return mNumRcds;}
	i64		dataLength() const {return mBodyBuffRaw->dataLen();} 
	i64		getMemSize() const
	{
		i64 size = mHeadBuffRaw->dataLen() + mBodyBuffRaw->dataLen();
		return size;
	}

	AosBuffPtr	getHeadBuff() {return mHeadBuff;}
	AosBuffPtr	getBodyBuff() {return mBodyBuff;}
	i64			getBodyAddr() {return mBodyAddr;}

	bool	appendEntry(
				const AosValueRslt &value, 
				AosRundata *rdata);
	bool 	appendEntry(
				AosDataRecordObj *record, 
				AosRundata *rdata);
	bool	appendEntry(
				const char *data,
				const int len,
				AosRundata *rdata);

	bool	insertEntry(
				const char *data,
				const int len,
				AosRundata *rdata);

	bool 	insertEntry(
				const i64 &idx,
				const char *data,
				const int len,
				AosRundata *rdata);

	bool	modifyEntry(
				const i64 &idx,
				const char* data,
				const int len,
				AosRundata *rdata);

	bool	getEntry(
				const i64 &idx,
				char * &data,
				int &len);

	i64		findPos(
				const i64 &idx,
				const char *data,
				const int len)
	{
		bool unique = false;
		return findPos(idx, data, len, unique);
	}

	i64		findPos(
				const i64 &idx,
				const char *data,
				const int len,
				bool &unique);
	
	bool	getDataFromBuff(const AosBuffPtr &buff);
	bool	setDataToBuff(const AosBuffPtr &buff);
	bool	setBodyToBuff(const AosBuffPtr &buff);
	bool	setBodyBuff(
				const AosBuffPtr &buff,
				int &remain_size,
				bool encode = true);

	AosBuffArrayVarPtr	split(const AosRundataPtr &rdata);

	int 	cmp(const AosBuffArrayVarPtr &lhs, const i64 &lidx,
				const AosBuffArrayVarPtr &rhs, const i64 &ridx);
	bool printBody();

	static bool buildHeaderBuff(
					vector<AosDataFieldType::E> &types,
					AosBuff *header_buff,
					char *body);

	static bool procHeaderBuff(
					const AosCompareFunPtr &comp, 
					const AosBuffPtr &headBuff, 
					AosBuffPtr &bodyBuff,
					set<i64> &bodyAddrSet,
					bool encode = true);

	bool	mergeData();

};

#endif

