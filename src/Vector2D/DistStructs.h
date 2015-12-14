/*
 * structs.h
 *
 *  Created on: Aug 21, 2015
 *      Author: root
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_


#pragma once


#pragma pack(8)
struct time_idx_entry
{
	u64 offset;				//we will change type to u32			
	u32 timeId;				//timeunit
	u32 distcount;			
	u32 buffLen;			//block size
	u32 dataLen;			//actual used block size
};

#pragma pack(4)
struct time_idx_table
{
	u32 size;
	struct time_idx_entry* entry;
};

#pragma pack(4)
struct doc_idx_entry
{
	u8 	valid;				// means is not exist
	u32 offset;				// time table offset
	u32 totalTimeSlotNum;	// total time num reserved
	u32 usedTimeSlotNum;	// actual time num used
	u32 buffLen;			// time block size
	u32 dataLen;			// actual used time block size
};

#pragma pack(8)
struct measure_idx_entry
{
	u8  measureIdx;		//  
	u64 offset;			// actual value offset
	u64 valueCount;		// value count
};

struct hole
{
	u64 offset;
	u64 size;
};


struct docid_lru
{

};

#pragma pack(4)
struct value_count
{
	u64 value;	//
//	u32 count;  //how many times the value appears
};



#endif /* STRUCTS_H_ */
