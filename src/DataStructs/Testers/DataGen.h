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
// 09/26/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataStructs_Testers_DataGen_h
#define Aos_DataStructs_Testers_DataGen_h

#include "DataStructs/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosDataGen : virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eMaxRecord = 50000,
		eInitEpochDay = 14610,
		eRecordLen = 324
	};


	struct Record
	{
		u64					binary_docid;
		OmnString 			field_key;
		u64		  			root_docid;
		map<u64, int64_t>	slots;

		void setSlot(const u64 &time_slot, const int64_t &vv)
		{
			if (slots.count(time_slot) == 0)
			{
				slots.insert(make_pair(time_slot, vv));
			}
			else
			{
				int64_t v1 = slots[time_slot] + vv;
				slots[time_slot] = v1;
			}
		}

		int64_t getSlot(const int64_t &time_slot)
		{
			if (slots.count(time_slot) == 0) return 0;
			return slots[time_slot];
		}

		u64 getSlotSize(){return slots.size();}

		void print()
		{
			map<u64, int64_t>::iterator itr = slots.begin();
			for (; itr != slots.end(); ++ itr)
			{
				OmnScreen << "DDDDDDDDDDDD:" << itr->first << ";"<< endl;
			}
		}
	};

private:
	vector<Record>		mRecord;
	AosBuffPtr			mBuff;
	AosBuffArrayPtr 	mArray;
	u64					mCrtDocid;
	AosRundataPtr 		mRdata;
	AosShuffleStatIdPtr	mShuffle;

public:
	AosDataGen();
	AosDataGen(
			const AosRundataPtr &rdata,
			const OmnString &control_objid);
	~AosDataGen();

	u64  getEpochDay();

	OmnString  getRandomStr();

	bool run();

	bool pickRcd();

	bool pickNewRcd();

	bool pickOldRcd();

	bool createBuffArray();

	bool getData(
		const AosRundataPtr &rdata,
		AosBuffPtr &data_buff);

	u64 getMaxDocid()
	{
		u64 max_docid = mCrtDocid -1;
		return max_docid;
	}

	bool isVaild(
			const u32 idx,
			const int64_t &time_slot,
			int64_t &vv);

	bool getRecord(
			vector<u64> &dist_block_docids,
			vector<u64> &stat_ids,
			map<u64, u32> &index,
			int64_t &s_time,
			int64_t &e_time);

	u64 getSlotSize(const u32 idx);
	void print(const u32 idx);
};
#endif
