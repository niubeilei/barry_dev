/*
 *  ZoneStructs.h
 *  Created by Phil on 2015-11-18 
 */
#ifndef ZONE_STRUCTS_H_
#define ZONE_STRUCTS_H_

///////////////////////////////////////////////
//  Entry structures
///////////////////////////////////////////////
//
//key_entry data strucut, including relevant
//time and value zone info
//
typedef struct key_entry 
{
	//ZoneIndex in the timeZone
	u32 timeZoneIndex;
	//total time entry reserved
	//if no time entry reserved, this key entry
	//is not valid
	u32 timeEntryAllocated;
	//actual time entry used
	u32 timeEntryCreated;

	//ZoneIndex in the valueZone
	u32 valueZoneIndex;
	//total value entry reserved
	u32 valueEntryAllocated;

} key_entry_t;

//
//time_entry data strucut, including relevant
//value zone info
//
typedef struct time_entry
{
	u32 timeId;

	//ZoneIndex in value zone
	//If no value entry allocated, the
	//time entry is not used
	u64 valueZoneIndex;
	u32 valueEntryAllocated;
	u32 valueEntryCreated;

} time_entry_t;

//
//value entry data struct, including distinct
//values and repeated number
//
typedef struct value_entry
{
	u64 value;
	u32 num;

} value_entry_t;

///////////////////////////////////////////////
//  Zone structures
///////////////////////////////////////////////

//
//value zone structure
//
typedef struct key_zone
{
	//Number of entries allocated
	u32 totalAllocated;

	//Total number of entries reserved. Some
	//of them might not be used
	u32 totalReserved;

} key_zone_t;

//
//value zone structure
//
typedef struct time_zone
{
	//Number of entries allocated to 
	//key entries
	u32 totalAllocated;

	//Total number of entries reserved. Some
	//of them might not be allocated to any
	//key entries
	u32 totalReserved;

} time_zone_t;

//
//value zone structure
//
typedef struct value_zone
{
	//Number of entries allocated to 
	//time entries
	u32 totalAllocated;

	//Total number of entries reserved. Some
	//of them might not be allocated to any
	//time entries
	u32 totalReserved;

} value_zone_t;


#endif /* ZONE_STRUCTS_H_ */
