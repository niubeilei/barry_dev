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
// 04/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcField_h
#define Aos_DataProc_DataProcField_h

#include "DataProc/DataProc.h"
#include "DataColComp/Ptrs.h"
#include "DataColComp/DataColComp.h"
#include "Util/DataTypes.h"


class AosDataProcField : virtual public AosDataProc
{
	OmnString           mKeyFieldName;
	int                 mKeyFieldIdx;
	AosDataColCompPtr	mColComp;
	
	Field				mField;		// Ketty 2014/01/15	

public:
	AosDataProcField(const bool flag);
	AosDataProcField(const AosDataProcField &proc);
	~AosDataProcField();

	virtual AosDataProcStatus::E procData(
				const AosDataRecordObjPtr &record,			
				const u64 &docid,
				const AosDataRecordObjPtr &output,			
				const AosRundataPtr &rdata);

	virtual AosDataProcObjPtr cloneProc();

	virtual AosDataProcObjPtr create(
						const AosXmlTagPtr &def, 
						const AosRundataPtr &rdata);
	
	virtual bool resolveDataProc(
	 					map<OmnString, AosDataAssemblerObjPtr> &asms,
 						const AosDataRecordObjPtr &record,
 						const AosRundataPtr &rdata);

	// Ketty 2013/12/19
	virtual bool getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos);	
	virtual AosDataProcStatus::E procData(
						AosRundata *rdata_raw,
						AosDataRecordObj **input_records,
						AosDataRecordObj **output_records);
	
private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);
	bool 	configV1(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);

};

#endif
