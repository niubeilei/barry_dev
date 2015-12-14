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
//
// Modification History:
// 12/07/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SemanticObj_ItfHashtab_h
#define Aos_SemanticObj_ItfHashtab_h

#include "SemanticObj/Ptrs.h"
#include "SemanticObj/SemanticObjCommon.h"

class AosHashtabInterface : public AosSemanticObjCommon
{
public:
	AosHashtabInterface(const std::string &name, 
						 const AosSOType::E type, 
						 void *inst)
	:
	AosSemanticObjCommon(name, type, inst)
	{
	}


	// 
	// Mandatory Interface
	//
	virtual u32 	getNoe() const = 0;
	virtual u32		getTableSize() const = 0;

	// 
	// Optional Interface
	//
	virtual u32		maxAllowedElem() const = 0;
	virtual u32		averageBucketLen() const = 0;
	virtual u32		bucketUsagePct() const = 0;
	virtual u32		maxBucketLen() const = 0;
	virtual u32		averageQueryTime() const = 0;
	virtual u32 	maxQueryTime() const = 0;
	virtual u32		averageAddTime() const = 0;
	virtual u32		maxAddTime() const = 0;
	virtual u32		averageDeleteTime() const = 0;
	virtual u32		maxDeleteTime() const = 0;
	virtual u32		totalQueries() const = 0;
	virtual u32		totalAdditions() const = 0;
	virtual u32		totalDeletions() const = 0;
};

#endif
