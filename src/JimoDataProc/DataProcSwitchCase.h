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
// 05/14/2015 Created by Barry
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_JimoDataProcSwitchCase_h
#define Aos_DataProc_JimoDataProcSwitchCase_h

#include "AosConf/DataRecordCtnr.h"
#include "AosConf/DataAssembler.h"

#include "DataRecord/Ptrs.h"
#include "JimoDataProc/JimoDataProc.h"

#include "JSON/JSON.h"


class AosJimoDataProcSwitchCase : public AosJimoDataProc
{
private:
	AosExprObjPtr       						mSwitchValue;
	AosExprObj*									mRawSwitchValue;

	map<OmnString, vector<AosDataProcObjPtr> >	mCaseProcs;
	map<OmnString, vector<AosDataProcObj*> >	mRawCaseProcs;
	vector<AosDataRecordObjPtr>					mInputRecords;

	AosValueRslt								mValue;


public:
	AosJimoDataProcSwitchCase(const int ver);
	AosJimoDataProcSwitchCase(const AosJimoDataProcSwitchCase &proc);
	~AosJimoDataProcSwitchCase();

	virtual AosDataProcStatus::E procData(
					AosRundata *rdata_raw,
					AosDataRecordObj **input_record,
					AosDataRecordObj **output_record);

	virtual bool createByJql(
					AosRundata *rdata,
					const OmnString &obj_name, 
					const OmnString &jsonstr,
					const AosJimoProgObjPtr &prog);

	virtual AosJimoPtr cloneJimo() const;
	virtual AosDataProcObjPtr cloneProc() ;
	virtual void setInputDataRecords(vector<AosDataRecordObjPtr> &records);

	bool 	start(const AosRundataPtr &rdata);
	bool 	finish(const vector<AosDataProcObjPtr> &procs,
				const AosRundataPtr &rdata);

	virtual int getMaxThreads() const;
private:
	bool	config(const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);

	bool 	getDataProcs(const JSONValue &json,
				const AosRundataPtr &rdata);

	AosDataProcObjPtr
			createProc(const JSONValue &dpJSON,
				const AosRundataPtr rdata);
};

#endif
