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
// 2013/05/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcStatDyn_h
#define Aos_DataProc_DataProcStatDyn_h

#include "DataProc/DataProc.h"
#include "DataColComp/Ptrs.h"
#include "DataColComp/DataColComp.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/DataTypes.h"
#include <vector>
using namespace std;

class AosDataProcStatDyn : virtual public AosDataProc
{
private:
	AosEntryCreatorObjPtr	mCreator;

public:
	AosDataProcStatDyn(const bool flag);
	AosDataProcStatDyn(const AosDataProcStatDyn &proc);
	~AosDataProcStatDyn();

	virtual AosDataProcStatus::E procData(
						const AosDataRecordObjPtr &record,			
						const u64 &docid,
						const AosDataRecordObjPtr &output,			
						const AosRundataPtr &rdata);
	virtual AosDataProcObjPtr cloneProc();
	virtual AosDataProcObjPtr create(
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);
	virtual bool	resolveDataProc(
						map<OmnString, AosDataAssemblerObjPtr> &asms,
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata);
	
	// Ketty 2013/12/19
	virtual AosDataProcStatus::E procData(
						AosRundata *rdata_raw,
						AosDataRecordObj **input_records,
						AosDataRecordObj **output_records);
	virtual bool 	resolveDataProc(
						const AosRundataPtr &rdata,
						const AosDataRecordObjPtr &input_record,
						const AosDataRecordObjPtr &output_record);
	
private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
};

#endif
