
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
#ifndef Aos_Vector2D_StatZone_h
#define Aos_Vector2D_StatZone_h

#include "Rundata/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/HashUtil.h"
#include "Vector2D/ZoneStruct.h"
#include <map>
#include "Vector2DUtil/Vt2dRecord.h"   

class AosStatZone : virtual public OmnRCObject 
{
	OmnDefineRCObject;

public:
	enum stat_type_t
	{
		eDist,
		eMax,
		eMin
	};

private:
	AosBuffPtr 			mBuff;
	AosBuffPtr 			mKeyTimeBuff;
	AosBuffPtr 			mValueBuff;
	stat_type_t 		mStatType;

	//different zone start
	key_zone_t*			mKeyZoneData;
	key_entry_t*		mKeyZone;
	time_zone_t*		mTimeZoneData;
	time_entry_t*		mTimeZone;
	value_zone_t*		mValueZoneData;
	value_entry_t*		mValueZone;

	//different indexes
	u64			mBaseDocId;
	u32			mKeyEntryPerDoc;
	u32			mKeyZoneSize;

public:
	AosStatZone(AosBuffPtr buff, u32 keyEntryPerDoc);
	~AosStatZone();
	//bool initZone();

	bool addValue(
			u64 docId,
			u32 timeId,
			u64 value);

	bool deleteValue(
			u64 docId,
			u32 timeId,
			u64 value);

	AosBuffPtr getBuff(){return mBuff;};
	void setBuff(AosBuffPtr &buff){mBuff = buff;refreshZones();};
	//static bool dumpData(AosDfmDocPtr &dfmDoc);

	bool readOneTimeEntry(
			AosRundata * rdata,
			AosVt2dRecord *rcd,
			const u64	&localSdocid,
			const i64	&timeId);
private:

	/////////////////////////////////////////////////
	//Block:  Init methods
	/////////////////////////////////////////////////
	bool initKeyEntry(key_entry_t *keyEntry);



	bool deleteTimeEntry(
			key_entry_t* keyEntry,
			int idx,
			u32 timeId);

	bool deleteValueEntry(
			key_entry_t* keyEntry,
			time_entry_t* timeEntry,
			int idx,
			u64 value);

	//location methods
	key_entry_t* getKeyEntry(u64 docId);

	time_entry_t* getTimeEntry(
			key_entry_t* keyEntry,
			u32 timeId,
			bool createFlag);

	value_entry_t* getValueEntry(
			key_entry_t  *keyEntry,
			time_entry_t* timeEntry,
			u64 value,
			bool createFlag);

	bool checkZoneSpace();

	bool initTimeEntry(time_entry_t *timeEntry);
	bool initValueEntry(value_entry_t *valueEntry);
	bool expandTimeZone();
	bool expandValueZone();

	///////////////////////////////////////////////////
	//Block:  Allocate entries:
	//	  Allocate more entries from the relevant 
	//	  zones. If no enough entires to allocate,
	//	  need to expand the zone
	///////////////////////////////////////////////////
	bool allocateTimeEntry(key_entry_t *keyEntry);
	bool allocateValueEntry(key_entry_t *keyEntry,time_entry_t *timeEntry);

	///////////////////////////////////////////////////
	//Block:  
	//       Create/delete entries
	///////////////////////////////////////////////////
	time_entry_t* createTimeEntry(
			key_entry_t* keyEntry,
			int idx,
			u32 timeId);

	value_entry_t* createValueEntry(
			key_entry_t* keyEntry,
			time_entry_t* timeEntry,
			int idx,
			u64 value);

	/////////////////////////////////////////////////
	//Block: 
	//      Get an entrry ntries from relevant zones
	/////////////////////////////////////////////////
	int getKeyIndex(u64 docId);

	void refreshZones();
};


#endif /* StatZone_H*/
