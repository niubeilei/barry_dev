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
// 2015/11/11 Updated by arvin
////////////////////////////////////////////////////////////////////////////
#include "StatUtil/StatModifyInfo.h"



AosStatModifyInfo::AosStatModifyInfo()
:
mOpr(0)
{

}

bool 
AosStatModifyInfo::addVt2dMdfInfo(
			const AosRundataPtr &rdata,
			const u32 vt2d_idx,
			AosVt2dInfo& vt2d_info)
{
	int vt2d_mdf_idx = -1;
	map<u32, u32>::iterator itr = mVt2dIdMap.find(vt2d_idx);
	if(itr == mVt2dIdMap.end())
	{
		AosVt2dModifyInfo info;
		info.mRecordType = mRecordType;
		info.mOpr = mOpr;

		info.initOutputDef(vt2d_idx,vt2d_info);

		vt2d_mdf_idx = mVt2dMdfInfos.size();
		mVt2dMdfInfos.push_back(info);
		mVt2dIdMap.insert(make_pair(vt2d_idx, vt2d_mdf_idx));
	}
	else
	{
		vt2d_mdf_idx = itr->second;
	}

	//aos_assert_r((u32)vt2d_mdf_idx < mVt2dMdfInfos.size(), false);

	return true;
}

bool 
AosStatModifyInfo::addBuff2Measure(
			const AosRundataPtr &rdata,
			const int idx,
			const u32 vt2d_idx,
			AosVt2dInfo &vt2d_info)
{
	int vt2d_mdf_idx = -1;
	map<u32, u32>::iterator itr = mVt2dIdMap.find(vt2d_idx);
	if(itr == mVt2dIdMap.end())
	{
		AosVt2dModifyInfo info;
		info.mRecordType = mRecordType;
		info.initOutputDef(vt2d_idx,vt2d_info);


		vt2d_mdf_idx = mVt2dMdfInfos.size();
		mVt2dMdfInfos.push_back(info);
		mVt2dIdMap.insert(make_pair(vt2d_idx, vt2d_mdf_idx));
	}
	else
	{
		vt2d_mdf_idx = itr->second;
	}

	aos_assert_r((u32)vt2d_mdf_idx < mVt2dMdfInfos.size(), false);

	mVt2dMdfInfos[vt2d_mdf_idx].addBuff2Measure(rdata,idx,vt2d_info);
	return true;
}

	
bool 
AosStatModifyInfo::addMeasure(
			const AosRundataPtr &rdata,
			const OmnString name,
			const int value_input_pos,
			const u32 value_data_len,
			const int time_input_pos,
			const u32 vt2d_idx,
			AosVt2dInfo &vt2d_info)
{
	int vt2d_mdf_idx = -1;
	map<u32, u32>::iterator itr = mVt2dIdMap.find(vt2d_idx);
	if(itr == mVt2dIdMap.end())
	{
		AosVt2dModifyInfo info;
		info.init(vt2d_idx, mSdocidPos, time_input_pos, 
				mRecordLen, vt2d_info);
		info.mOpr = mOpr;
		info.mRecordType = mRecordType;

		vt2d_mdf_idx = mVt2dMdfInfos.size();
		mVt2dMdfInfos.push_back(info);
		mVt2dIdMap.insert(make_pair(vt2d_idx, vt2d_mdf_idx));
	}
	else
	{
		vt2d_mdf_idx = itr->second;
	}

	aos_assert_r((u32)vt2d_mdf_idx < mVt2dMdfInfos.size(), false);
	aos_assert_r(mVt2dMdfInfos[vt2d_mdf_idx].mTimeFieldPos
			== time_input_pos, false);
	mVt2dMdfInfos[vt2d_mdf_idx].addMeasure(rdata, name, 
			value_input_pos, value_data_len, vt2d_info);	
	return true;
}

//yang
bool 
AosStatModifyInfo::addMeasure(
				const AosRundataPtr &rdata,
				const int idx,
				const int value_input_pos,
				const u32 value_data_len,
				const int time_input_pos,
				const u32 vt2d_idx,
				AosVt2dInfo &vt2d_info)
{
	int vt2d_mdf_idx = -1;
	map<u32, u32>::iterator itr = mVt2dIdMap.find(vt2d_idx);
	if(itr == mVt2dIdMap.end())
	{
		AosVt2dModifyInfo info;
		info.init(vt2d_idx, mSdocidPos, time_input_pos,
				mRecordLen, vt2d_info);
		info.mOpr = mOpr;
		vt2d_mdf_idx = mVt2dMdfInfos.size();
		mVt2dMdfInfos.push_back(info);
		mVt2dIdMap.insert(make_pair(vt2d_idx, vt2d_mdf_idx));
	}
	else
	{
		vt2d_mdf_idx = itr->second;
	}

	aos_assert_r((u32)vt2d_mdf_idx < mVt2dMdfInfos.size(), false);
	aos_assert_r(mVt2dMdfInfos[vt2d_mdf_idx].mTimeFieldPos
			== time_input_pos, false);
	mVt2dMdfInfos[vt2d_mdf_idx].addMeasure(rdata, idx,
			value_input_pos, value_data_len, vt2d_info);
	return true;
}


vector<AosVt2dModifyInfo> & 
AosStatModifyInfo::getVt2dMdfInfos()
{ 
	return mVt2dMdfInfos; 
}

bool 
AosStatModifyInfo::serializeTo(
			const AosRundataPtr &rdata,
			const AosBuffPtr &buff)
{
	buff->setU32(mRecordType);
	buff->setU64(mMeasureType);
	if(mRecordType != AosDataRecordType::eBuff)
	{
		buff->setU64(mRecordLen);
		buff->setInt(mSdocidPos);
		buff->setInt(mKeyPos);
		buff->setInt(mIsNewFlagPos);
		//buff->setInt(mTimeFieldPos);

		buff->setOmnStr(mOpr);

		bool rslt;
		buff->setU32(mVt2dMdfInfos.size());
		for(u32 i=0; i<mVt2dMdfInfos.size(); i++)
		{
			rslt = mVt2dMdfInfos[i].serializeTo(rdata, buff);
			aos_assert_r(rslt, false);
		}
	}
	else
	{
		//not need recordlen,docid,key,and isnewflag info
		buff->setOmnStr(mOpr);

		bool rslt;
		buff->setU32(mVt2dMdfInfos.size());
		for(u32 i=0; i<mVt2dMdfInfos.size(); i++)
		{
			rslt = mVt2dMdfInfos[i].serializeTo(rdata, buff);
			aos_assert_r(rslt, false);
		}
	}
	return true;
}
	
bool 
AosStatModifyInfo::serializeFrom(
			const AosRundataPtr &rdata,
			const AosBuffPtr &buff)
{
	mRecordType = (AosDataRecordType::E)buff->getU32(0);
	mMeasureType = (E)buff->getU64(0);
	if(mRecordType != AosDataRecordType::eBuff)
	{
		mRecordLen = buff->getU64(0);
		aos_assert_r(mRecordLen, false);

		mSdocidPos = buff->getInt(-1);
		aos_assert_r(mSdocidPos != -1, false);

		mKeyPos = buff->getInt(-1);
		aos_assert_r(mKeyPos != -1, false);

		mIsNewFlagPos = buff->getInt(-1);
		aos_assert_r(mIsNewFlagPos != -1, false);

		mOpr = buff->getOmnStr("");

		//mTimeFieldPos = buff->getInt(-1);
		//aos_assert_r(mTimeFieldPos != -1, false);
		bool rslt;
		u32 vt2d_mdf_num = buff->getU32(0);
		AosVt2dModifyInfo mdf_info;
		for(u32 i=0; i<vt2d_mdf_num; i++)
		{
			mdf_info.mOpr = mOpr;
			mdf_info.mRecordType = mRecordType;
			rslt = mdf_info.serializeFrom(rdata, buff);
			aos_assert_r(rslt, false);
			mVt2dMdfInfos.push_back(mdf_info);
		}
	}
	else
	{
		mOpr = buff->getOmnStr("");

		bool rslt;
		u32 vt2d_mdf_num = buff->getU32(0);
		AosVt2dModifyInfo mdf_info;
		for(u32 i=0; i<vt2d_mdf_num; i++)
		{
			mdf_info.mOpr = mOpr;
			mdf_info.mRecordType = mRecordType;
			mdf_info.mMeasureType = (AosVt2dModifyInfo::E)mMeasureType;

			rslt = mdf_info.serializeFrom(rdata, buff);
			aos_assert_r(rslt, false);
			mVt2dMdfInfos.push_back(mdf_info);
		}
	}
	return true;
}
	
void 
AosStatModifyInfo::clear()
{
	mRecordLen = 0;
	mSdocidPos = -1;
	mKeyPos = -1;
	mIsNewFlagPos = -1;

	mVt2dMdfInfos.clear();
	mVt2dIdMap.clear();
	mMeasureType = INVALID;
}

