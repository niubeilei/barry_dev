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
// 2015/09/20 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Synchers_SyncherSumMapExecutor_h
#define AOS_Synchers_SyncherSumMapExecutor_h

#include "SEInterfaces/SyncherType.h"
#include "SEInterfaces/DataFieldType.h"
#include "Synchers/Syncher.h"
#include "SEUtil/DeltaBeanOpr.h"


class AosSyncherSumMapExecutor : public AosSyncher
{
private:
	OmnString 				mIILName;	// _zt4g_idxmgr_...

public:
	AosSyncherSumMapExecutor(const int version);
	virtual ~AosSyncherSumMapExecutor();

	virtual AosSyncherType::E getType() {return AosSyncherType::eSumMapExecutor;}
	virtual bool proc();
	virtual AosBuffPtr serializeToBuff();
	virtual bool serializeFromBuff(const AosBuffPtr &buff);
	virtual AosJimoPtr cloneJimo() const;
	virtual int getDestCubeID() { return 0; }


public:
	bool	proc(AosRundata *rdata, const AosDatasetObjPtr &dataset);
	bool	procData(AosRundata *rdata, AosDataRecordObj *record);
	bool 	incrementIndex(AosRundata *rdata,
					const AosIILObjPtr &iilobj,
					const OmnString &key,
					const u64 &incvalue);


};

#endif
