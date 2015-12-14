////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights Allocated.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// 	This file implements key-time-value zone for the following statistics
//		distinct count
//		max
//		min
//
// Modification History:
// 2015/11/18 Created by Phil
////////////////////////////////////////////////////////////////////////////
#include "Vector2D/StatZone.h"

#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "DfmUtil/DfmDoc.h"

//////////////////////////////////////////////////////
//         Define needed constants
//////////////////////////////////////////////////////
static char const sgInitChar = 0xCC;
//static char const sgInitChar = 0x0;
static u32 const sgBigEnough = 2000000;
//static u32 const mKeyEntryPerDoc = 20;  //fixed
//static u32 const sgKeyZoneSize = mKeyEntryPerDoc * sizeof(key_entry_t);  //fixed

//when need to reserve/expand more entries, allocate 
//a number of a group at once
//static u32 const sgTimeEntryPerGroup = 20;
static u32 const sgTimeEntryPerGroup = 40;
//static u32 const sgValueEntryPerGroup = 20;
static u32 const sgValueEntryPerGroup = 60;

//when need to allocate more time entry, allocate a
//group of entries at once
static u32 const sgTimeEntryPerExpand = 1000 * sgTimeEntryPerGroup;
static u32 const sgValueEntryPerExpand = 10000 * sgValueEntryPerGroup;

//different counters
static u64 sgTotalAppendNum = 0;
static u64 sgTotalMoveNum = 0;
static u64 sgTotalMoveSize = 0;
static u64 sgTotalExpandNum = 0;
static u64 sgTotalExpandSize = 0;

/////////////////////////////////////////////////
//Block:  constructor/destructor
/////////////////////////////////////////////////
AosStatZone::AosStatZone(AosBuffPtr buff, u32 keyEntryPerDoc)
:
mKeyEntryPerDoc(keyEntryPerDoc)
{
	//Read the doc from DFM firstly
	
	//Not found, need to create a new doc
	//create the buff and set default value
	if(buff)
	{
		mBuff = buff;
	}
	else
	{
		u32 keyZoneSize = mKeyEntryPerDoc * sizeof(key_entry_t) + 
			sizeof(key_zone_t) + sizeof(time_zone_t) + sizeof(value_zone_t);
		mBuff = OmnNew AosBuff(keyZoneSize AosMemoryCheckerArgs);
		//memset((void*)mBuff->data(), sgInitChar, keyZoneSize);
		memset((void*)mBuff->data(), 0, keyZoneSize);

		//init zone headers

#if 0
		char *data = mBuff->data();

		mKeyZoneData = (key_zone_t*)data;
		mKeyZoneData->totalAllocated = 0;
		mKeyZoneData->totalReserved = keyEntryPerDoc;

		mTimeZoneData = (time_zone_t*)(data + sizeof(key_zone_t));
		mTimeZoneData->totalAllocated = 0;
		mTimeZoneData->totalReserved = 0;

		mValueZoneData = (value_zone_t*)(data 
				+ sizeof(key_zone_t) + sizeof(time_zone_t));
		mValueZoneData->totalAllocated = 0;
		mValueZoneData->totalReserved = 0;
#endif
	}

	refreshZones();
	//mBaseDocId = doc->getDocid();
 	mKeyZoneSize = mKeyEntryPerDoc * sizeof(key_entry_t);
	mStatType = eDist;
}

AosStatZone::~AosStatZone()
{
}

//
//Each time the buff is changed or expaneded,
//this method needs to be called
//
void
AosStatZone::refreshZones()
{
	aos_assert(mBuff);
	char *data = mBuff->data();

	mKeyZoneData = (key_zone_t*)data;
	mKeyZone = (key_entry_t*)(data + sizeof(key_zone_t) 
			+ sizeof(time_zone_t) + sizeof(value_zone_t));

	mTimeZoneData = (time_zone_t*)(data + sizeof(key_zone_t));
	mTimeZone = (time_entry_t*)((char*)mKeyZone + 
			+ sizeof(key_entry_t) * mKeyEntryPerDoc); 

	mValueZoneData = (value_zone_t*)(data 
			+ sizeof(key_zone_t) + sizeof(time_zone_t));
	mValueZone = (value_entry_t*)((char*)mTimeZone + 
			+ sizeof(time_entry_t) * mTimeZoneData->totalReserved);

}

/////////////////////////////////////////////////
//Block:  APIs  add/delete methods
//      Exposed to external
/////////////////////////////////////////////////
bool 
AosStatZone::addValue(
		u64 docId,
		u32 timeId,
		u64 value)
{
	key_entry_t* keyEntry;
	time_entry_t* timeEntry;
	value_entry_t* valueEntry;
	bool rslt;

	sgTotalAppendNum++;

	//If zone space is not enough, expand zone firstly
	rslt = checkZoneSpace();
	aos_assert_r(rslt, false);

#if 0
bool debugFlag = true;
while(sgTotalAppendNum == 100001 && debugFlag)
{
rslt = true;	
}
#endif
	keyEntry = getKeyEntry(docId);
	if (!keyEntry)
	{
		OmnAlarm << "DocId:" << docId 
			<< " is not in the doc with the base docid: " 
			<< mBaseDocId << enderr;

		return false;
	}
	
	//Add the time and value in the keyEntry
	//If no this entry, just create one for me
	//in the right position.
	timeEntry = getTimeEntry(keyEntry, timeId, true);
	aos_assert_r(timeEntry, false);

	valueEntry = getValueEntry(keyEntry, timeEntry, value, true);
	aos_assert_r(valueEntry, false);
	
	//valueEntry->value = value;	
	valueEntry->num++;
	return true;
}

bool 
AosStatZone::deleteValue(
		u64 docId,
		u32 timeId,
		u64 value)
{
	key_entry_t* keyEntry;
	time_entry_t* timeEntry;
	value_entry_t* valueEntry;

	keyEntry = getKeyEntry(docId);
	if (!keyEntry)
	{
		OmnAlarm << "DocId:" << docId 
			<< " is not in the doc with the base docid: " 
			<< mBaseDocId << enderr;

		return false;
	}

	if (!keyEntry->timeEntryCreated)
	{
		//No value, do nothing
		return true;
	}

	timeEntry = getTimeEntry(keyEntry, timeId, true);
	if (!timeEntry)
	{
		//Not found and do nothing
		return true;
	}

	valueEntry = getValueEntry(keyEntry, timeEntry, value, true);
	if (!valueEntry)
	{
		//Not found and do nothing
		return true;
	}
	
	//Found the entry, decrement
	valueEntry->num--;
	if (!valueEntry->num)
	{
		//free the value entry
		//If the time entry has no other entry,
		//the time entry can also be reclaimed
	}
	
	return true;
}

bool
AosStatZone::readOneTimeEntry(
		AosRundata * rdata,
		AosVt2dRecord *rcd,
		const u64	&localSdocid,
		const i64	&timeId)
{
	key_entry_t *keyEntry;
	time_entry_t *timeEntry;

	keyEntry = getKeyEntry(localSdocid);
	aos_assert_r(keyEntry, false);

	value_entry_t *valueEntry;
	u32 vt2dEntrySize = 1 + sizeof(u64);
	
	if(timeId == -1)
	{
		//means read all time in the keyEntry
		timeEntry = mTimeZone + keyEntry->timeZoneIndex;
		u32 timeEntryNum = keyEntry->timeEntryCreated;
		for(size_t i = 0; i < timeEntryNum; i++,timeEntry++)
		{
			u32 valueEntryNum = timeEntry->valueEntryCreated;
			u64 valSize = valueEntryNum * sizeof(u64);
			AosBuff* tmpBuf = OmnNew AosBuff((8 + valSize)  AosMemoryCheckerArgs);

			tmpBuf->setU64(valueEntryNum);//set value count
			valueEntry = mValueZone + keyEntry->valueZoneIndex + timeEntry->valueZoneIndex;
			for(size_t j = 0; j < valueEntryNum; j++,valueEntry++)
			{
				//tmpBuf hold values without the count
				tmpBuf->setU64(valueEntry->value);
			}
			AosBuffPtr vt2dSubEntry = OmnNew AosBuff(9 AosMemoryCheckerArgs);
			AosBuffPtr vt2dEntry = OmnNew AosBuff(vt2dEntrySize AosMemoryCheckerArgs);

			vt2dSubEntry->setChar('1');
			vt2dSubEntry->setU64((u64)&(tmpBuf->data()[0]));
			vt2dEntry->setBuff(vt2dSubEntry);

			rcd->appendValue(timeEntry->timeId, vt2dEntry->data(), vt2dEntrySize);
		}
	}
	else
	{
		timeEntry = getTimeEntry(keyEntry, timeId, false);
		if (!timeEntry) 
			return true;

		u32 valueEntryNum = timeEntry->valueEntryCreated;
		u64 valSize = valueEntryNum * sizeof(u64);
		AosBuff* tmpBuf = OmnNew AosBuff(8 + valSize  AosMemoryCheckerArgs);

		tmpBuf->setU64(valueEntryNum);//set value count
		valueEntry = mValueZone + keyEntry->valueZoneIndex + timeEntry->valueZoneIndex;
		for(size_t j = 0; j < valueEntryNum; j++,valueEntry++)
		{
			//tmpBuf hold values without the count
			tmpBuf->setU64(valueEntry->value);
		}

		AosBuffPtr vt2dSubEntry = OmnNew AosBuff(9 AosMemoryCheckerArgs);
		AosBuffPtr vt2dEntry = OmnNew AosBuff(vt2dEntrySize AosMemoryCheckerArgs);
		vt2dSubEntry->setChar('1');
		vt2dSubEntry->setU64((u64)&(tmpBuf->data()[0]));
		vt2dEntry->setBuff(vt2dSubEntry);

		rcd->appendValue(timeEntry->timeId, vt2dEntry->data(), vt2dEntrySize);
	}

	return true;
}

/////////////////////////////////////////////////
//Block:  Init methods
/////////////////////////////////////////////////

//init keyEntry
bool
AosStatZone::initKeyEntry(key_entry_t *keyEntry)
{
	aos_assert_r(keyEntry, false);

	keyEntry->timeEntryCreated = 0;
	keyEntry->timeEntryAllocated = 0;
	keyEntry->timeZoneIndex = 0;
	keyEntry->valueZoneIndex = 0;
	keyEntry->valueEntryAllocated = 0;
	return true;	
}

//init timeEntry
bool
AosStatZone::initTimeEntry(time_entry_t *timeEntry)
{
	aos_assert_r(timeEntry, false);

	timeEntry->timeId = 0;
	timeEntry->valueZoneIndex = 0;
	timeEntry->valueEntryAllocated = 0;
	timeEntry->valueEntryCreated = 0;
	return true;	
}

//init timeEntry
bool
AosStatZone::initValueEntry(value_entry_t *valueEntry)
{
	aos_assert_r(valueEntry, false);

	valueEntry->value = 0;
	valueEntry->num = 0;
	return true;	
}

/////////////////////////////////////////////////
//Block:  Expanding methods, expand mBuff space
//     Key Zone needs no expanding
/////////////////////////////////////////////////

//
//Expanding time zone to hold more 
//time entries
//
bool
AosStatZone::expandTimeZone()
{
	//aos_assert_r(keyEntry, false);

	bool rslt;
	u32 oldLen = mBuff->buffLen();
	//how much space to expand
	u32 num = sgTimeEntryPerExpand;
	u32 delta = num * sizeof(time_entry_t);

	//mBuff->setDataLen(oldLen);
	rslt = mBuff->expandMemoryCopyAll(oldLen + delta);
	aos_assert_r(rslt, false);

	//update zones
	refreshZones();

	//Move forward value zone data
	void* src = (void*)mValueZone;
	void* dst = src + delta;
	u32 moveLen = mValueZoneData->totalReserved * sizeof(value_entry_t);
	memmove(dst, src, moveLen);

	//init the newly reserved time entry data
	memset(src, sgInitChar, delta);

	//update relevant references
	mValueZone = (value_entry_t*)((char*)mValueZone + delta);
	mTimeZoneData->totalReserved += sgTimeEntryPerExpand;
	
	//update relevant counters
	sgTotalExpandNum++;
	sgTotalExpandSize += delta;
	sgTotalMoveNum++;
	sgTotalMoveSize += moveLen;
	return true;
}

//
//Expand value zone to hold more value
//entries
//
bool
AosStatZone::expandValueZone()
{
	bool rslt;
	u32 oldLen = mBuff->buffLen();
	//how much space to expand
	u32 num = sgValueEntryPerExpand;
	u32 delta = num * sizeof(value_entry_t);

	//mBuff->setDataLen(oldLen);
	//i64 newLen = oldLen + delta;
	rslt = mBuff->expandMemoryCopyAll(oldLen + delta);
	aos_assert_r(rslt, false);

	//update zones
	refreshZones();

	//init the newly reserved time entry data
	memset((void*)(mBuff->data() + oldLen), sgInitChar, delta);

	//update relevant references
	mValueZoneData->totalReserved += sgValueEntryPerExpand;

	//update relevant counters
	sgTotalExpandNum++;
	sgTotalExpandSize += delta;
	return true;
}

//
//Just make sure time zone and value zone
//has extra space for new allocation
//
bool
AosStatZone::checkZoneSpace()
{
	bool rslt;

	aos_assert_r(mValueZoneData->totalReserved 
			>= mValueZoneData->totalAllocated, false);
	if (mValueZoneData->totalReserved ==
			mValueZoneData->totalAllocated)
	{
		rslt = expandValueZone();
		aos_assert_r(rslt, false);
	}

	aos_assert_r(mTimeZoneData->totalReserved 
			>= mTimeZoneData->totalAllocated, false);
	if (mTimeZoneData->totalReserved ==
			mTimeZoneData->totalAllocated)
	{
		rslt = expandTimeZone();
		aos_assert_r(rslt, false);
	}

	return true;
}

///////////////////////////////////////////////////
//Block:  Allocate entries:
//	  Allocate more entries from the relevant 
//	  zones. If no enough entires to allocate,
//	  need to expand the zone
///////////////////////////////////////////////////

//
//Allocate more time entry space for a key entry
//
//If the zone space is not enough, return failure 
//
bool
AosStatZone::allocateTimeEntry(key_entry_t *keyEntry)
{
	aos_assert_r(keyEntry, false);
	aos_assert_r(mTimeZoneData->totalReserved 
			>= mTimeZoneData->totalAllocated, false);

	bool rslt;

	//Zone space should be enough due to checkZoneSpace()
	//at the beginning
	aos_assert_r(mTimeZoneData->totalReserved > 
			mTimeZoneData->totalAllocated, false);

	//Allocate more time entries for the key entry
	//at the end of the key entry's time area
	//
	//Prepare the key entry firstly
	if (!keyEntry->timeEntryAllocated)
	{
		//need to preapre the empty key entry
		initKeyEntry(keyEntry);

		//Has no time entry before
		if (keyEntry != mKeyZone)
		{
			//not the first entry
			key_entry_t* prevEntry = keyEntry - 1;
			aos_assert_r(prevEntry->timeEntryAllocated, false);

			keyEntry->timeZoneIndex = 
				prevEntry->timeZoneIndex + prevEntry->timeEntryAllocated;
			aos_assert_r(keyEntry->timeZoneIndex <= 
					mTimeZoneData->totalAllocated, false);
		}
	}

	//Make sure that the time entries are really used up
	aos_assert_r(keyEntry->timeEntryAllocated 
			== keyEntry->timeEntryCreated, false);

	//Need to move forward subsequent time entries,starting
	//from the end of the current key entry's time area
	u32 lastTimeEntry = keyEntry->timeZoneIndex + 
		keyEntry->timeEntryAllocated;
	time_entry_t* src = mTimeZone + lastTimeEntry;
	time_entry_t* dst = src + sgTimeEntryPerGroup;
	time_entry_t* end = mTimeZone + mTimeZoneData->totalAllocated;
	u32 moveLen = (char*)end - (char*)src;
	memmove((void*)dst, (void*)src, moveLen);

	//init the new entry space
	u32 delta = (char*)dst - (char*)src;
	memset((void*)src, sgInitChar, delta);

	//update the subsequent keys' timeZoneIndex
	u32 firstIndex = keyEntry - mKeyZone + 1;		
	u32 lastIndex = mKeyEntryPerDoc;
	for (u32 i = firstIndex; i < mKeyEntryPerDoc; i++)
	{
		mKeyZone[i].timeZoneIndex += delta;
	}

	//update relevant references
	keyEntry->timeEntryAllocated += sgTimeEntryPerGroup;
	mTimeZoneData->totalAllocated += sgTimeEntryPerGroup; 
	
	//update counters
	sgTotalMoveNum++;
	sgTotalMoveSize += moveLen;
	return true;
}

//
//Allocate more value entry space for a time entry
//from the value zone
//
//If the zone space is not enough, return failure 
//
bool
AosStatZone::allocateValueEntry(
		key_entry_t *keyEntry,
		time_entry_t *timeEntry)
{
	aos_assert_r(keyEntry && timeEntry, false);
	aos_assert_r(mValueZoneData->totalReserved 
			>= mValueZoneData->totalAllocated, false);

	bool rslt;

	//Zone space should be enough due to checkZoneSpace()
	//at the beginning
	aos_assert_r(mValueZoneData->totalReserved > 
			mValueZoneData->totalAllocated, false);

	//Allocate more value entries for the time entry
	//at the end of the time entry's value area
	//
	//Prepare the time entry firstly
	if (!timeEntry->valueEntryAllocated)
	{
		//Need to check if keyEntry's valueZoneIndex
		//allocated or not firstly
		if (!keyEntry->valueEntryAllocated)
		{
			//init the keyEntry's valueEntry info
			keyEntry->valueZoneIndex = mValueZoneData->totalAllocated;
		}

		//Has no value entry before
		//If the timeEntry is the first one of a Key entry
		// 1. its valueZoneIndex = 0
		// 2. Coming here, the timeEntry has been inited before
		// 3. So its default valueZoneIndex = 0
		// 4. Therefore no need to assign a new value for the
		//    the first timeEntry of a key entry
		if (timeEntry != mTimeZone + keyEntry->timeZoneIndex)
		{
			//not the first entry
			time_entry_t* prevEntry = timeEntry - 1;
			aos_assert_r(prevEntry->valueEntryAllocated, false);

			timeEntry->valueZoneIndex = 
				prevEntry->valueZoneIndex + prevEntry->valueEntryAllocated;
			aos_assert_r(keyEntry->valueZoneIndex + timeEntry->valueZoneIndex 
					<= mValueZoneData->totalAllocated, false);	
		}
	}

	//Make sure that the value entries are really used up
	aos_assert_r(timeEntry->valueEntryAllocated 
			== timeEntry->valueEntryCreated, false);

	//Need to move forward subsequent value entries,starting
	//from the end of the current time entry's value area
	u32 lastValueEntry = keyEntry->valueZoneIndex + 
		timeEntry->valueZoneIndex + timeEntry->valueEntryAllocated;
	value_entry_t* src = mValueZone + lastValueEntry;
	value_entry_t* dst = src + sgValueEntryPerGroup;
	value_entry_t* end = mValueZone + mValueZoneData->totalAllocated;
	u32 moveLen = (char*)end - (char*)src;
	memmove((void*)dst, (void*)src, moveLen);

	//init the new entry space
	u32 delta = dst - src;
	memset((void*)src, sgInitChar, delta * sizeof(value_entry_t));

	//Update the subsequent times' valueZoneIndex
	//in the key entry
	u32 firstIndex = timeEntry - mTimeZone + 1;		
	u32 lastIndex = keyEntry->timeZoneIndex + 
						keyEntry->timeEntryCreated;
	for (u32 i = firstIndex; i < lastIndex; i++)
	{
		mTimeZone[i].valueZoneIndex += delta;
	}

	//Update the subsequent keys' valueZoneIndex
	firstIndex = keyEntry - mKeyZone + 1;		
	lastIndex = mKeyEntryPerDoc;
	for (u32 i = firstIndex; i < lastIndex; i++)
	{
		mKeyZone[i].valueZoneIndex += delta;
	}

	//update relevant references
	keyEntry->valueEntryAllocated += sgValueEntryPerGroup;
	timeEntry->valueEntryAllocated += sgValueEntryPerGroup;
	mValueZoneData->totalAllocated += sgValueEntryPerGroup; 
	
	//update counters
	sgTotalMoveNum++;
	sgTotalMoveSize += moveLen;
	return true;
}

///////////////////////////////////////////////////
//Block:  
//       Create/delete entries
///////////////////////////////////////////////////
//
//Add a new time entry for a key entry.
//Assume the key has enough time entry
//slot
//
time_entry_t*
AosStatZone::createTimeEntry(
		key_entry_t* keyEntry,
		int idx,
		u32 timeId)
{
	aos_assert_r(keyEntry, NULL);
	aos_assert_r(keyEntry->timeEntryCreated <= keyEntry->timeEntryAllocated, NULL);

	bool rslt;
	if (keyEntry->timeEntryCreated == keyEntry->timeEntryAllocated)
	{
		rslt = allocateTimeEntry(keyEntry);
		//Coming here
		// 1. The space should be enough due to
		//     checkZoneSpace() at the beginning
		// 2. The Allocation should be correct
		//     Too
		aos_assert_r(rslt, NULL);
	}

	time_entry_t *timeEntry = 
		mTimeZone + keyEntry->timeZoneIndex + idx;

	if (idx < keyEntry->timeEntryCreated)
	{
		//The new time entry should be in the middle
		//Need to push forward subsequent time entry
		void *src = (void*)timeEntry; 
		void *dst =	src + sizeof(time_entry_t); 
		//u32 moveLen = (keyEntry->timeEntryCreated - idx + 1) 
		//	* sizeof(time_entry_t);
		u32 moveLen = (keyEntry->timeEntryCreated - idx) 
			* sizeof(time_entry_t);

		memmove(dst, src, moveLen);
		sgTotalMoveNum++;
		sgTotalMoveSize += moveLen;
	}

	//Set the new time entry
	initTimeEntry(timeEntry);
	timeEntry->timeId = timeId;

	//update references
	keyEntry->timeEntryCreated++;
	return timeEntry;
}

bool
AosStatZone::deleteTimeEntry(
		key_entry_t* keyEntry,
		int idx,
		u32 timeId)
{
	return true;
}

//
//Create a new value entry for a time entry.
//Assume the timeEntry has enough value entry
//slot
//
value_entry_t*
AosStatZone::createValueEntry(
		key_entry_t* keyEntry,
		time_entry_t* timeEntry,
		int idx,
		u64 value)
{
	aos_assert_r(keyEntry && timeEntry, NULL);
	aos_assert_r(timeEntry->valueEntryCreated <= timeEntry->valueEntryAllocated, NULL);

	bool rslt;
	if (timeEntry->valueEntryCreated == timeEntry->valueEntryAllocated)
	{
		//No enough value entry slot for creation. 
		//Need to allocate more 
		rslt = allocateValueEntry(keyEntry, timeEntry);
		//Coming here
		// 1. The space should be enough due to
		//     checkZoneSpace() at the beginning
		// 2. The Allocation should be correct
		//     Too
		aos_assert_r(rslt, NULL);
	}

	value_entry_t *valueEntry = mValueZone + 
		keyEntry->valueZoneIndex + timeEntry->valueZoneIndex + idx;

	if (idx < timeEntry->valueEntryCreated)
	{
		//The new time entry should be in the middle
		//Need to push forward subsequent value entry
		void *src = (void*)valueEntry; 
		void *dst =	src + sizeof(value_entry_t); 
		//u32 moveLen = (timeEntry->valueEntryCreated - idx + 1) 
		//	* sizeof(value_entry_t);
		u32 moveLen = (timeEntry->valueEntryCreated - idx) 
			* sizeof(value_entry_t);

		memmove(dst, src, moveLen);
		sgTotalMoveNum++;
		sgTotalMoveSize += moveLen;
	}

	valueEntry->value = value;
	//This requires the caller increment the num
	valueEntry->num = 0;

	timeEntry->valueEntryCreated++;
	return valueEntry;
}

bool
AosStatZone::deleteValueEntry(
		key_entry_t* keyEntry,
		time_entry_t* timeEntry,
		int idx,
		u64 value)
{
	return true;
}

/////////////////////////////////////////////////
//Block: 
//      Get an entrry ntries from relevant zones
/////////////////////////////////////////////////
//
//Return the index of a docid
//     -1: means not existing
//
int
AosStatZone::getKeyIndex(u64 docId)
{
	//int idx = docId - mBaseDocId;
	int idx = (docId - 1) % mKeyEntryPerDoc;

	if (idx >= 0 && idx < mKeyEntryPerDoc)
	{
		return idx;
	}

	return -1;
}

//
//Return a pointer to a time entry
//Return value:
//     Null: means the time slot 
//     not existing
//
key_entry_t*
AosStatZone::getKeyEntry(u64 docId)
{
	int idx = getKeyIndex(docId);
	key_entry_t* keyEntry = NULL;

	if (idx >= 0)
	{
		keyEntry = (key_entry_t*)(&mKeyZone[idx]); 
		aos_assert_r(keyEntry->timeEntryAllocated 
				>= keyEntry->timeEntryCreated, NULL);
	}

	return keyEntry;
}

//
//Return a pointer to a time entry in a key entry
//
//Params:
//	   keyEntry: the parent keyEntry
//     timeId: the time entry's Id 
//     createFlag: whether or not need to create
//         a new timeEntry if not existing. This
//         will fail if the all the Allocated 
//         time entries are Created up.
//
//Return value:
//	   If createFlag = false:
//     		Null: means the time entry not existing
//     		Otherwise return the found timeEntry
//
//        createFlag = true:
//          Null: means the time entry not existing
//               and failed to create a new one due
//               to less of keyEntry's time entries  
//          Otherwise return the found timeEntry or
//               a newly created timeEntry if not 
//               found.
//
time_entry_t*
AosStatZone::getTimeEntry(
		key_entry_t *keyEntry,
		u32 timeId,
		bool createFlag)
{
	aos_assert_r(keyEntry, NULL);

	//get the time ZoneIndex
	if (!keyEntry->timeEntryAllocated)
	{
		//The key has no time entry slot Allocated.
		//Not to allocate spaces firstly
		return createTimeEntry(keyEntry, 0, timeId);
	}

	time_entry_t *timeEntry = mTimeZone + keyEntry->timeZoneIndex;
	int idx;
	for (idx = 0; idx < keyEntry->timeEntryCreated; idx++, timeEntry++)
	{
		if (timeEntry->timeId >= timeId)
		{
			if (timeEntry->timeId == timeId)
				return timeEntry;  //found the timeId

			//This one is bigger, if need to create a new one
			//it will be inserted here
			break;
		}
	}

	// Not found and not need to create
	if (!createFlag)
		return NULL; 

	//Add a new time entry
	timeEntry = createTimeEntry(keyEntry, idx, timeId);
	return timeEntry;
}

//
//Return a pointer to a value entry in a time entry
//
//Params:
//	   timeEntry: the parent timeEntry
//     value: the value of the value entry
//     createFlag: whether or not need to create
//         a new valueEntry if not existing. This
//         will fail if the all the Allocated 
//         value entries are Created up.
//
//Return value:
//	   If createFlag = false:
//     		Null: means the value entry not existing
//     		Otherwise return the found valueEntry
//
//        createFlag = true:
//          Null: means the value entry not existing
//               and failed to create a new one due
//               to less of timeEntry's value entries  
//          Otherwise return the found valueEntry or
//               a newly created valueEntry if not 
//               found.
//
value_entry_t*
AosStatZone::getValueEntry(
		key_entry_t *keyEntry,
		time_entry_t *timeEntry,
		u64 value,
		bool createFlag)
{
	aos_assert_r(keyEntry && timeEntry, NULL);

	//get the time ZoneIndex
	if (!timeEntry->valueEntryAllocated)
	{
		//The key has no time entry slot Allocated.
		//Not to allocate spaces firstly
		return createValueEntry(keyEntry, timeEntry, 0, value);
	}

	value_entry_t *valueEntry = mValueZone + 
		+ keyEntry->valueZoneIndex + timeEntry->valueZoneIndex;
	int idx;
	for (idx = 0; idx < timeEntry->valueEntryCreated; idx++, valueEntry++)
	{
		if (valueEntry->value >= value)
		{
			if (valueEntry->value == value)
				return valueEntry;  //found the value

			//This one is bigger, if need to create a new one
			//it will be inserted here
			break;
		}
	}

	// Not found and not need to create
	if (!createFlag)
		return NULL; 

	//Add a new time entry
	valueEntry = createValueEntry(keyEntry, timeEntry, idx, value);
	return valueEntry;
}

