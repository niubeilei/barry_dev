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
// 05/31/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcIf_h
#define Aos_DataProc_DataProcIf_h

#include "DataProc/DataProc.h"
#include "DataRecord/Ptrs.h"
#include "SEUtil/SeTypes.h"


class AosDataProcIf : virtual public AosDataProc
{
	AosConditionObjPtr			mFilter;
	vector<AosDataProcObjPtr>	mTrueProcs;
	vector<AosDataProcObjPtr>	mFalseProcs;
		
public:
	AosDataProcIf(const bool flag);
	AosDataProcIf(const AosDataProcIf &proc);
	~AosDataProcIf();

	virtual AosDataProcStatus::E procData( 		// will delete later.
						const AosDataRecordObjPtr &record,
						const u64 &docid,
						const AosDataRecordObjPtr &output,
						const AosRundataPtr &rdata);
	virtual bool	resolveDataProc(			// will delete later.
						map<OmnString, AosDataAssemblerObjPtr> &asms,
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata);
	
	// Ketty 2013/12/19
	//virtual bool 	resolveDataProc(
	//					const AosRundataPtr &rdata,
	//					const AosDataRecordObjPtr &input_record,
	//					const AosDataRecordObjPtr &output_record);
	virtual bool getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos);	
	virtual AosDataProcStatus::E procData(
						AosRundata *rdata_raw,
						AosDataRecordObj **input_record,
						AosDataRecordObj **output_record);
	
	virtual AosDataProcObjPtr cloneProc();
	virtual AosDataProcObjPtr create(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	
};

#endif

