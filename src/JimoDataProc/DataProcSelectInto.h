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
// 12/24/2014 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoDataProc_DataProcSelectInto_h
#define Aos_JimoDataProc_DataProcSelectInto_h

#include "AosConf/DataRecord.h"

#include "SEInterfaces/RecordFieldInfo.h"
#include "DataRecord/Ptrs.h"
#include "Util/File.h"
#include "JimoDataProc/JimoDataProc.h"
#include "JSON/JSON.h"

class AosDataProcSelectInto : public AosJimoDataProc
{
	enum
	{
		eMaxBuffSize = 10000000 //10M
	};
private:
	AosDataRecordObjPtr 				mOutputRecord;
	vector<AosDataRecordObjPtr>			mInputRecords;
public:
	AosDataProcSelectInto(const int ver);
	AosDataProcSelectInto(const AosDataProcSelectInto &proc);
	~AosDataProcSelectInto();

	virtual AosDataProcStatus::E procData(
			AosRundata *rdata_raw,
			AosDataRecordObj **input_record,
			AosDataRecordObj **output_record);

	virtual AosJimoPtr 		cloneJimo() const;
	virtual AosDataProcObjPtr cloneProc() ;

	virtual bool	start(const AosRundataPtr &rdata);
	virtual bool	finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata);
	virtual bool	getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos);
	bool 			createByJql(
						AosRundata *rdata,
						const OmnString &obj_name,
						const OmnString &jsonstr);
	void setInputDataRecords(vector<AosDataRecordObjPtr> &records);
private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);

	bool			createOutput(
						const OmnString &dpname,
						const JSONValue &json_conf,
						const AosRundataPtr &rdata);
};

#endif
