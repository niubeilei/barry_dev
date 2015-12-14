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
// 03/04/2015 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcWordCount_h
#define Aos_DataProc_DataProcWordCount_h


#include "JimoDataProc/JimoDataProc.h"
#include "SEInterfaces/RecordFieldInfo.h"
#include "WordParser/WordParser.h"

class AosDataProcWordCount : public AosJimoDataProc
{
private:
	vector<OmnString>			mKeys;
	map<OmnString, int> 		mWordMap;

public:
	AosDataProcWordCount(const int ver);
	~AosDataProcWordCount();

	virtual AosDataProcStatus::E procData(
			AosRundata *rdata_raw,
			AosDataRecordObj **input_record,
			AosDataRecordObj **output_record);

	virtual AosJimoPtr 		cloneJimo() const;
	virtual AosDataProcObjPtr cloneProc() ;

	virtual bool	finish(const vector<AosDataProcObjPtr> &prcos, const AosRundataPtr &rdata);
	virtual bool	getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos);
	bool 			createByJql(
						AosRundata *rdata,
						const OmnString &obj_name,
						const OmnString &jsonstr,
						const AosJimoProgObjPtr &prog);

private:
	bool			config(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
};

#endif
