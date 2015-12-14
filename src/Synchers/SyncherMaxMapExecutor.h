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
// 2015/11/03 Created by Andy 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Synchers_SyncherMaxMapExecutor_h
#define AOS_Synchers_SyncherMaxMapExecutor_h

#include "SEInterfaces/SyncherType.h"
#include "SEInterfaces/DataFieldType.h"
#include "Synchers/Syncher.h"
#include "SEUtil/DeltaBeanOpr.h"
#include "Util/DataTypes.h"


class AosSyncherMaxMapExecutor : public AosSyncher
{
private:
	OmnString				mKeyIILName;
	OmnString 				mMapIILName;
	AosDataType::E			mDataType;

public:
	AosSyncherMaxMapExecutor(const int version);
	virtual ~AosSyncherMaxMapExecutor();

	virtual AosSyncherType::E getType() {return AosSyncherType::eMaxMapExecutor;}
	virtual bool proc();
	virtual AosBuffPtr serializeToBuff();
	virtual bool serializeFromBuff(const AosBuffPtr &buff);
	virtual AosJimoPtr cloneJimo() const;
	virtual int getDestCubeID() { return 0; }


public:
	bool	proc(AosRundata *rdata, const AosDatasetObjPtr &dataset);
	bool	procData(AosRundata *rdata, AosDataRecordObj *record);

	AosDeltaBeanOpr::E pickOprByAdd(
					AosRundata* rdata,
					const AosValueRslt &map_key, 
					const AosValueRslt &map_value,
					AosValueRslt &old_key,
					AosValueRslt &old_value);

	AosDeltaBeanOpr::E pickOprByDelete(
					AosRundata* rdata,
					const AosValueRslt &map_key,
					const AosValueRslt &map_value,
					AosValueRslt &old_key,
					AosValueRslt &old_value);

	AosDeltaBeanOpr::E pickOprByModify(
					AosRundata* rdata,
					const AosValueRslt &map_key,
					const AosValueRslt &map_value,
					AosValueRslt &old_key,
					AosValueRslt &old_value);

	bool operationIIL(
					AosRundata *rdata,
					const AosDeltaBeanOpr::E opr,
					const OmnString &iil_name,
					const u64 &docid,
					const AosValueRslt &new_value,
					const AosValueRslt &old_key,
					const AosValueRslt &old_value);

	int compare(const AosValueRslt lhs, u64	rhs);
	bool setValue(AosValueRslt &value, u64 docid);

};

#endif
