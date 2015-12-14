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
// 2014/07/21	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StatTrans_BatchSaveStatDocsTrans_h
#define Aos_StatTrans_BatchSaveStatDocsTrans_h

#include "TransUtil/StatTrans.h"
#include "StatUtil/StatModifyInfo.h"

class AosBatchSaveStatDocsTrans : virtual public AosStatTrans
{

private:
	AosXmlTagPtr mStatCubeConf;
	AosBuffPtr	mInputData;
	AosStatModifyInfo mStatMdfInfo;

	AosXmlTagPtr mRecordBuff2Conf;
	AosDataRecordObjPtr mRecordBuff2;

public:
	AosBatchSaveStatDocsTrans(const bool regflag);
	AosBatchSaveStatDocsTrans(
			const u32 cube_id,
			const AosXmlTagPtr &stat_conf,
			const AosBuffPtr &input_data,
			AosStatModifyInfo &stat_mdf_info);


	~AosBatchSaveStatDocsTrans();

	// Trans Interface	
	virtual bool serializeFrom(const AosBuffPtr &buff);
	virtual bool serializeTo(const AosBuffPtr &buff);
	virtual AosTransPtr clone();
	virtual bool proc();

private:
	bool 	setErrResp();

};
#endif

