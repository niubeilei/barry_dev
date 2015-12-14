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
#ifndef Aos_Sorter_DataSourceVar_h
#define Aos_Sorter_DataSourceVar_h

#include "Sorter/DataSource.h"

class DataSourceVar : public DataSource 
{
private:
	AosBuffPtr 			mIncompleteData;
	AosBuff				*mIncompleteDataRaw;
	AosBuffPtr 			mHeadBuff;
	AosBuff*			mHeadBuffRaw;

public:
	DataSourceVar(AosNetFileObjPtr &file, int buffSize, AosCompareFun *cmp);
	DataSourceVar(const AosBuffPtr &buff, AosCompareFun *cmp);
	DataSourceVar(AosCompareFun *cmp);

	virtual bool getEntry(DataSource *value);
	virtual char* 	getData();
	virtual char* 	getHeadBuff();
	virtual void	procHeadBuff(); 
	virtual void 	moveNext();
	virtual int 	getEntryLen();
	bool 			checkIsCompleteEntry();

	virtual void fileReadCallBack(
						const u64 &reqId, 
						const int64_t &expected_size, 
						const bool &finished, 
						const AosDiskStat &disk_stat);

};
#endif

