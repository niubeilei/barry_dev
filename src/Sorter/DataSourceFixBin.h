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
// 10/27/2012 Created by Ice Yu
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Sorter_DataSourceFixBin_h
#define Aos_Sorter_DataSourceFixBin_h

#include "Rundata/Ptrs.h"
#include "Sorter/DataSource.h"

class DataSourceFixBin : public DataSource 
{
	u64		mTime;

public:
	DataSourceFixBin(AosNetFileObjPtr &file, int buffSize, AosCompareFun *cmp);
	DataSourceFixBin(const AosBuffPtr &buff, AosCompareFun *cmp); 
	DataSourceFixBin(AosCompareFun *cmp);

	virtual char* getData();
	virtual char* getHeadBuff();
	virtual int getEntryLen();
	virtual void moveNext();

	virtual void fileReadCallBack(
				const u64 &reqId, 
				const int64_t &expected_size, 
				const bool &finished, 
				const AosDiskStat &disk_stat);

};
#endif

