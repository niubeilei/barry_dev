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
// 2013/12/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataStructs_RootStatDoc_h
#define Aos_DataStructs_RootStatDoc_h

#include "DataStructs/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "IILUtil/IILUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"

class AosRootStatDoc : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:

	enum
	{
		eIndexNumOff = 0,
		eIndexHeaderLen = sizeof(u64),
		eIndexStartOff = eIndexHeaderLen,
		eIndexRcdStartOff = 0, 
		eIndexRcdDocidOff = sizeof(u64),
		eIndexRcdLen = sizeof(u64) + sizeof(u64),

		eGeneralValueOff = 0,
		eGeneralValueLen = sizeof(u64),
		eDftKeyLen = AosIILUtil::eMaxStrValueLen,
		eKeyLenOff = 0,
		eKeyLenSize = sizeof(u32),
		eKeyOff = eKeyLenOff + eKeyLenSize 

	};

public:
	enum LongType
	{
		eInvExtStatId = 0xffffffffffffffffLL,
		eInvNumIndex = 0xffffffffffffffffLL
	};

public:
	struct Parameter
	{
		friend class AosRootStatDoc;
	private:
		int64_t					mValuesSize;
		int64_t					mIndexBodySize;
		u64 					mTimeBucketWeight;
		AosDataType::E			mDataType;

		u64						mMaxNumIndex;
		int						mDftValue;

	public:
		Parameter()
		:
		mValuesSize(0),
		mIndexBodySize(0),
		mTimeBucketWeight(0),
		mDataType(AosDataType::eInvalid),
		mMaxNumIndex(0),
		mDftValue(0)
		{
		}

		Parameter(
			const int64_t &index_size,
			const int64_t &values_size,
			const u64 &time_bucket_weight,
			const AosDataType::E data_type,
			const int dft)
		:
		mValuesSize(values_size),
		mIndexBodySize(0),
		mTimeBucketWeight(time_bucket_weight),
		mDataType(data_type),
		mMaxNumIndex(0),
		mDftValue(dft)
		{
			mIndexBodySize = index_size - eIndexHeaderLen;
			mMaxNumIndex = mIndexBodySize / eIndexRcdLen;
		}

		~Parameter()
		{
		}

		u64 getTimeBucketWeight() const {return mTimeBucketWeight;}
		int getDftValue() const {return mDftValue;}
	};

	struct ValueRslt
	{
		bool				is_new;
		u64					ext_docid;
		u64					ext_slot_idx;

		bool				kickout;
		vector<u64>			kick_extdocid;
		vector<AosBuffPtr>  kick_statdocs;

		ValueRslt()
		:
		ext_docid(0),
		ext_slot_idx(0),
		kickout(false)
		{
			kick_extdocid.clear();
			kick_statdocs.clear();
		}

		void setExtInfo(
				const u64 &id, 
				const u64 &idx,
				const bool new_flag = false)
		{
			is_new = new_flag;
			ext_docid = id;
			ext_slot_idx = idx;
		}

		void addKickInfo(const u64 &id, const AosBuffPtr &buff)
		{
			kickout = true;
			aos_assert(kick_extdocid.size() == kick_statdocs.size());
			kick_extdocid.push_back(id);
			kick_statdocs.push_back(buff);
		}

		bool isKickOut() {return kickout;}

		~ValueRslt(){}
	};


protected:
	char * 					mData;
	int64_t 				mDataLen;

	char *					mIndexBodyData;

	u64						mNumIndex;
	int64_t					mGeneral;
	OmnString				mKey;
	ValueRslt				mValueRslt;

	u64						mStatId;
	Parameter				mParm;

public:
	AosRootStatDoc();

	AosRootStatDoc(
			char *data,
			const int64_t &data_len,
			const Parameter &parm,
			const u64 &stat_id,
			const OmnString &key);

	AosRootStatDoc(
			char *data,
			const int64_t &data_len,
			const Parameter &parm);

	~AosRootStatDoc();

	bool modifyRootStatDoc(
			const AosRundataPtr &rdata, 
			const u64 &idx, 
			const int64_t &value,
			const int vid,
			const AosStructProcObjPtr &proc,
			bool &out_range);

	bool getStatDocIndex(
			const AosRundataPtr &rdata, 
			const AosBuffPtr &buff,
			const u64 &idx, 
			u64 &ext_docid,
			u64 &start_idx);

	ValueRslt getValueRslt(){return mValueRslt;}

	OmnString getStatKey() {return mKey;}

	int64_t getStatGeneral() {return mGeneral;}

	bool getRootStatDoc(const AosBuffPtr &buff);

	bool  getAllStatDocIndex(
			const AosRundataPtr &rdata,
			vector<u64> &ext_docids,
			vector<u64> &start_idxs);

private:
	bool findBlockIdxByIndex(
			const u64 &value,
			u64 &ext_docid,
			u64 &start_idx,
			u64 &index_seqno);

	u64 findBoundary(
			const char *array,
			const int64_t &data_len,
			const u64 &value);

	AosBuffPtr	kickoutBucket(
			const AosRundataPtr &rdata,
			const int vid,
			const u64 &new_root_start_idx,
			u64 &old_root_extdid, bool &out_range);

	bool setOldRootDocidByIndex(
			const u64 &idx,
			const u64 &start_idx,
			const u64 &ext_docid);

	AosBuffPtr getKickOutBucket();

	bool appendNewRootIndex(
			const u64 &idx,
			const u64 &start_idx);

	void setIndexRcd(
			const int64_t pos,
			const u64 &start_idx,
			const u64 &docid);

	u64  getIndexRcdStartIdx(const int64_t &pos);

	u64  getIndexRcdDocid(const int64_t pos);

	void setGeneralValue(const AosStructProcObjPtr &proc, const int64_t &value);
	bool isValid(const AosBuffPtr &kick_bucket);

	bool modifyOldExtDocidByIndex(
			const u64 &index_seqno,
			const u64 &check_start_idx,
			const u64 &ext_docid);

	bool getIndexByIndexSeqno(
			u64 &index_seqno,
			u64 &ext_docid,
			u64 &start_idx);

	bool sanityCheck();

	void print(
			const char *array,
			const int64_t &data_len,
			const u64 &num_index);
};
#endif



