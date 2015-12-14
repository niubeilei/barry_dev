////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 10/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Sorter_SortCaller_h
#define Aos_Sorter_SortCaller_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/SorterType.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosSortCaller : virtual public OmnRCObject
{
public:
	virtual bool reduceCacheSize(const int64_t size, const bool merged) {return true;};
	virtual bool mergeSortFinish(
			const AosBuffPtr &buff, 
			const int level, 
			void *data,
			const AosRundataPtr &rdata) = 0;
	virtual void mergeSortFinish(const void* sorter, const int level) {};
};

#endif

