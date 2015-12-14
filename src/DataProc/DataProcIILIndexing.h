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
#ifndef Aos_DataProc_DataProcIILIndexing_h
#define Aos_DataProc_DataProcIILIndexing_h

#include "DataProc/DataProc.h"
#include "SEUtil/SeTypes.h"


#define AosDataProcIILIndexingTimeProfile \
	static u64 lsTime[8] = {0, 0, 0, 0, 0, 0, 0, 0}; \
    static u32 lsTick = 0; \
    if (lsTick++ >= 1000000) \
    { \
		cout << __FILE__ << ":" << __LINE__ \
			<< ": =============TimeProfiling=======================\n"\
			<< "        Time0: " << lsTime[0] << "\n"  \
			<< "        Time1: " << lsTime[1] << "\n"  \
			<< "        Time2: " << lsTime[2] << "\n"  \
			<< "        Time3: " << lsTime[3] << "\n"  \
			<< "        Time4: " << lsTime[4] << "\n"  \
			<< "        Time5: " << lsTime[5] << "\n"  \
			<< "        Time6: " << lsTime[6] << "\n"  \
			<< "        Time7: " << lsTime[7] << endl; \
		lsTick = 0; \
	}
						                                                                             
class AosDataProcIILIndexing : virtual public AosDataProc
{
	OmnString			mKeyFieldName;
	int					mKeyFieldIdx;
	int					mDocidFieldIdx;
	
	OmnString			mOutputFieldName;		// Ketty 2014/01/09
	int					mOutputFieldIdx;

	AosIILType			mIILType;
	AosConditionObjPtr	mFilter;

public:
	AosDataProcIILIndexing(const bool flag);
	AosDataProcIILIndexing(const AosDataProcIILIndexing &proc);
	~AosDataProcIILIndexing();

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
