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
#ifndef AOS_Synchers_SyncherIndexExecutor_h
#define AOS_Synchers_SyncherIndexExecutor_h

#include "SEInterfaces/SyncherType.h"
#include "SEInterfaces/DataFieldType.h"
#include "Synchers/Syncher.h"
#include "SEUtil/DeltaBeanOpr.h"


class AosSyncherIndexExecutor : public AosSyncher
{
private:
	OmnString 				mIILName;	// _zt4g_idxmgr_...

public:
	AosSyncherIndexExecutor(const int version);
	virtual ~AosSyncherIndexExecutor();

	virtual AosSyncherType::E getType() {return AosSyncherType::eIndexExecutor;}
	virtual bool proc();
	virtual AosBuffPtr serializeToBuff();
	virtual bool serializeFromBuff(const AosBuffPtr &buff);
	virtual AosJimoPtr cloneJimo() const;
	virtual int getDestCubeID() { return 0; }

public:
	bool	proc(AosRundata *rdata, const AosDatasetObjPtr &dataset);

private:
	bool			updateIndex(AosRundata *rdata, 
						const AosDeltaBeanOpr::E opr, 
						const AosValueRslt &key_v,
						const u64 docid);
	AosIILType 		getIILType(const AosDataType::E type);
	bool			procData(AosRundata *rdata, AosDataRecordObj *record);
	bool 			addIndex(AosRundata *rdata, 
						const AosIILObjPtr &iilobj,
						const AosValueRslt &vv, 
						const u64 &docid, 
						const bool value_unique,                          
						const bool docid_unique);
	bool 			deleteIndex(AosRundata *rdata, 
						const AosIILObjPtr &iilobj,
						const AosValueRslt &value, 
						const u64 &docid);
	bool 			modifyIndex(AosRundata *rdata, 
						const AosIILObjPtr &iilobj,
						const AosValueRslt &oldvalue, 
						const AosValueRslt &newvalue, 
						const u64 &docid,
						const bool value_unique,
						const bool docid_unique);

	bool 			generatorOutput(AosRundata *rdata, 
						const AosDeltaBeanOpr::E opr, 
						const AosValueRslt &oldvalue, 
						const AosValueRslt &newvalue, 
						const u64 &docid);
	AosDataRecordObjPtr generatorDataRecord(AosRundata *rdata,
						const AosDeltaBeanOpr::E opr, 
						const AosDataFieldType::E keytype);

};

#endif
