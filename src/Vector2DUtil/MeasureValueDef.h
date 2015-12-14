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
// 2014/07/25 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Vector2DUtil_MeasureValueDef_h
#define AOS_Vector2DUtil_MeasureValueDef_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "aosUtil/Types.h"
#include "Rundata/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Util/DataTypes.h"
#include "XmlUtil/XmlTag.h"

#include <vector>
using namespace std;

struct AosMeasureValueDef
{
	struct Entry
	{
		AosDataType::E mDataType;
		u32			mDataLen;		// not contain valid flag.
		u32			mDataPos;		// contain valid flag.
	};

	bool			mHasValidFlag;  // true means has null flag or not
	u32				mValueSize;
	vector<Entry>	mEntrys;
	
	AosMeasureValueDef()
	:
	mHasValidFlag(false),
	mValueSize(0)
	{
	}

	OmnString toXmlString(const AosRundataPtr &rdata)
	{
		OmnString str = "<m_value_def ";
		str << "has_valud_flag=\"" << (mHasValidFlag ? "true" : "false") << "\" " 
			<< "value_size=\"" << mValueSize << "\" " 
			<< ">";

		str << "<defs>";
		for(u32 i=0; i<mEntrys.size(); i++)
		{
			str << "<entry data_type=\"" << AosDataType::getTypeStr(mEntrys[i].mDataType) << "\" "
				<< "data_len=\"" << mEntrys[i].mDataLen << "\" "
				<< "data_pos=\"" << mEntrys[i].mDataPos << "\" "
				<< "></entry>";
		}
		str << "</defs>";

		str << "</m_value_def>";
		return str;
	}
	
	bool 	config(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &conf)
	{
		aos_assert_r(conf, false);
		
		mHasValidFlag = conf->getAttrBool("has_valud_flag", false);
		mValueSize = conf->getAttrU32("value_size", 0); 
		
		AosXmlTagPtr entrys_conf = conf->getFirstChild("defs", true);
		aos_assert_r(entrys_conf, false);
		
		Entry entry;
		OmnString data_type_str;
		AosXmlTagPtr each_conf = entrys_conf->getFirstChild(true);
		while(each_conf)
		{
			data_type_str = each_conf->getAttrStr("data_type", "");
			entry.mDataType = AosDataType::toEnum(data_type_str);
			entry.mDataLen = each_conf->getAttrU32("data_len", 0);
			entry.mDataPos = each_conf->getAttrU32("data_pos", 0);

			mEntrys.push_back(entry);
			each_conf = entrys_conf->getNextChild();
		}
		return true;
	}

	bool add(const AosDataType::E data_type)
	{
		// this func will reset mValueSize.
		Entry tt;
		tt.mDataType = data_type;
		tt.mDataLen = AosDataType::getValueSize(data_type);
		tt.mDataPos = mValueSize;
		if(mHasValidFlag)
		{
			//tt.mDataPos++;	// ketty 2014/12/05
			mValueSize++;
		}
		mEntrys.push_back(tt);
		
		mValueSize += tt.mDataLen;
		return true;
	};


	bool add(const AosDataType::E data_type, const int pos, const u32 datalen)
	{
		// this func will not reset mValueSize.
		u32 expect_data_len = AosDataType::getValueSize(data_type);
		aos_assert_r(datalen <= expect_data_len, false);
		
		Entry tt;
		tt.mDataType = data_type;
		tt.mDataPos = pos;
		tt.mDataLen = datalen;
		aos_assert_r(pos + tt.mDataLen <= mValueSize, false);
		mEntrys.push_back(tt);
		
		return true;
	}
	
	bool add2()
	{
		Entry tt;
		mEntrys.push_back(tt);
		return true;
	}

	u32	getValueNum(){ return mEntrys.size(); };


	inline bool getValue(
			char *data,
			const u32 data_len,
			const u32 idx,
			char * &value_data,
			AosDataType::E &value_type,
			u32 &value_len,
			bool &is_valid)
	{
		aos_assert_r(idx < mEntrys.size(), false);	
		aos_assert_r(data_len == mValueSize, false);
		
		value_len = mEntrys[idx].mDataLen;
		value_type = mEntrys[idx].mDataType;
		if(mHasValidFlag)
		{
			is_valid = ((data + mEntrys[idx].mDataPos)[0] != 0);
			value_data = data + mEntrys[idx].mDataPos + 1; 
			return true;
		}
		
		is_valid = true;
		value_data = data + mEntrys[idx].mDataPos; 
		return true;
	}

	inline bool setValue(
			char *data,
			const u32 data_len,
			const u32 idx,
			char *value_data,
			const u32 &value_len)
	{
		aos_assert_r(idx < mEntrys.size(), false);	
		aos_assert_r(data_len <= mValueSize, false);
		aos_assert_r(value_len <= mEntrys[idx].mDataLen, false);
		
		u32 real_value_loc = mEntrys[idx].mDataPos;
		if(mHasValidFlag)
		{
			(data + mEntrys[idx].mDataPos)[0] = 1;
			real_value_loc++;
		}
		
		memcpy(data + real_value_loc, value_data, value_len);
		if(mEntrys[idx].mDataLen > value_len)
		{
			memset(data + real_value_loc + value_len, 0,
					mEntrys[idx].mDataLen - value_len); 
		}
		return true;
	}

	/*
	inline bool setValueValid(
			char *data,
			const u32 data_len,
			const u32 idx)
	{
		aos_assert_r(idx < mEntrys.size(), false);	
		aos_assert_r(data_len <= mValueSize, false);
		
		if(mHasValidFlag)
		{
			(data + mEntrys[idx].mDataPos)[0] = 1;
		}
		return true;
	}
	*/

	bool	 serializeTo(const AosBuffPtr &buff)
	{
		buff->setU8(mHasValidFlag);
		buff->setU32(mValueSize);
		buff->setU32(mEntrys.size());
		for(u32 i=0; i<mEntrys.size(); i++)
		{
			buff->setU32(mEntrys[i].mDataType);
			buff->setU32(mEntrys[i].mDataLen);
			buff->setU32(mEntrys[i].mDataPos);
		}
		return true;
	}

	bool 	serializeFrom(const AosBuffPtr &buff)
	{
		mHasValidFlag = buff->getU8(0);
		mValueSize = buff->getU32(0);
		
		u32 entry_num = buff->getU32(0);
		Entry ent;
		for(u32 i=0; i<entry_num; i++)
		{
			ent.mDataType = (AosDataType::E)buff->getU32(0);	
			ent.mDataLen = buff->getU32(0);
			ent.mDataPos = buff->getU32(0);
			mEntrys.push_back(ent);
		}
		return true;
	}
};

#endif
