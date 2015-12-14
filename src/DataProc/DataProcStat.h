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
#ifndef Aos_DataProc_DataProcStat_h
#define Aos_DataProc_DataProcStat_h

#include "DataProc/DataProc.h"
#include "SEUtil/SeTypes.h"


class AosDataProcStat : virtual public AosDataProc
{
	vector<OmnString>	mKeyFieldNameArray;
	vector<int>			mKeyFieldIdxArray;
	OmnString 			mValueFieldName;
	int 				mValueFieldIdx;
	OmnString			mTimeFieldName;
	int					mTimeFieldIdx;

	AosConditionObjPtr	mFilter;
	OmnString			mSep;
	char 				mStatType;

	OmnString				mOutputFieldName;		// Ketty 2014/01/09
	int						mOutputFieldIdx;

public:
	AosDataProcStat(const bool flag);
	AosDataProcStat(const AosDataProcStat &proc);
	~AosDataProcStat();

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

	// Chen Ding, 12/25/2012
	// bool	compseKeyStr(
	// 			vector<OmnString> key_strs,
	// 			const AosDataRecordObjPtr &record,
	// 			const AosRundataPtr &rdata);

};

#endif
