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
// 09/13/2011	Created by Linda Lin
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILUtil_IILId_h
#define AOS_IILUtil_IILId_h

#include "API/AosApi.h"


#define AOSIILID_SYSTEM_DOCIDS					500
#define AOSIILID_CLOUDID						1600
#define AOSIILID_IMAGEID						1601
#define AOSIILID_COMPID							1602
#define AOSIILID_SSID							1603
#define AOSIILID_REMOTEBACK_TRANSID				1604
#define AOSIILID_VIRTUALDIR						1605
#define AOSIILID_COUNTERNAMES					1606
#define AOSIILID_COUNTER_IILIDS					1607
#define AOSIILID_SIZEIDS						1608
#define AOSIILID_SIZE_DOCIDS					1609
#define AOSZTG_SESSIONIILID						1610
#define AOSIILID_DOCSCANNERID					1611
#define AOSIILID_STATID							1612
#define AOSIILID_SIZEIDS_RECORD					1613
#define AOSIILID_DOC_GROUP_DGDIDS				1700	// Chen Ding, 2015/07/29
#define AOSIILID_ACTIVE_DGDIDS					10000	// Chen Ding, 2015/07/30
														// IMPORTANT: The next 65535*2 IILIDs
														// shall not be used for other purposes.

class AosIILID
{
private:
	static u64 	smDGDIDIncValue;

	enum
	{
		eReservedDGDID = 100
	};

public:
	inline static u64 getDocGroupDGDIDIILID(const int cube_id)
	{
		// This IIL is used to manage the active DGDIDs for all doc
		// groups. Its format is:
		// 		[container_docid + sizeid, active_dgdid]
		// 'active_dgdid' is the DGDID the container is using for the 
		// cube 'cube_id'. If the entry does not exist, it means 
		// the container does not have the doc group for the cube ID
		// yet. 
		//
		// DGDIDs are always in the form:
		// 		eReservedDGDID*#Cubes*M + CubeID
		// where M is the size of doc groups.
		//
		// The IIL's initial value is: eReservedDGDID*M*N + CubeID.
		// Its increment value is M*N. 
		return AOSIILID_DOC_GROUP_DGDIDS;
	}

	inline static u64 getDGDIDInitValue(const u32 group_size, const int cube_id)
	{
		aos_assert_r(group_size > 0, 0);
		aos_assert_r(cube_id >= 0, 0);
		return eReservedDGDID * group_size * AosGetNumCubes() + cube_id;
	}

	inline static u64 getDGDIDIncValue(const u32 group_size)
	{
		aos_assert_r(group_size > 0, 0);
		return group_size * AosGetNumCubes();
	}

	inline static u64 getDGDIDIILID()
	{
		// This IIL is used to allocate DGDIDs. Its format is:
		// 		[group_size + cube_id, next_dgdid]
		// Any time when one needs a new DGDID, it should use
		// this IIL. Note that JimoDB supports multiple sizes. 
		// The key is, therefore, identified by group size and
		// cube ID.
		//
		// If a DGDID is N, it manages the following docids:
		// 		N	(the group doc itself)
		// 		N + 1*#Cube,
		// 		N + 2*#Cube,
		// 		...
		// 		N + (GroupSize-1)*#Cube,
		//
		// In other word, when allocating one dgdid, we are actually
		// allocating GroupSize number of docids. All these docids
		// will be in the same cube.
		//
		// The Init value for this IIL is obtained by calling
		// 'getDGDIDInitValue(group_size)' and its increment value by
		// calling 'getDGDIDIncValue(group_size)'.
		return AOSIILID_ACTIVE_DGDIDS;
	}
};
#endif
