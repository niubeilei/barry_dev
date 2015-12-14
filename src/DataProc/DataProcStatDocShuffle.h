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
// 04/01/2014 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcStatDocShuffle_h
#define Aos_DataProc_DataProcStatDocShuffle_h

#include "DataProc/DataProc.h"
#include "DataStructs/Ptrs.h"

class AosDataProcStatDocShuffle : virtual public AosDataProc
{
	
	struct OutputInfo 
	{
		RecordFieldInfo				mIdOutput;	//maybe statDocid or cubeId	
		RecordFieldInfo				mStatKeyOutput;	
		//RecordFieldInfo				mStatTimeOutput;	
		vector<RecordFieldInfo>		mStatTimeOutputs;	
		vector<RecordFieldInfo>		mStatValueOutputs;	
	};
		
	RecordFieldInfo				mStatKeyInput;	
	RecordFieldInfo				mStatDocidInput;	// just mShuffleByCube will set.
	//RecordFieldInfo				mStatTimeInput;	
	vector<RecordFieldInfo>		mStatTimeInputs;	
	vector<RecordFieldInfo>		mStatValueInputs;
	
	vector<OutputInfo>			mShuffleInfo;
	bool						mShuffleByCube;

public:
	AosDataProcStatDocShuffle(const bool flag);
	AosDataProcStatDocShuffle(const AosDataProcStatDocShuffle &proc);
	~AosDataProcStatDocShuffle();

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

	virtual bool getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos);	

	virtual AosDataProcStatus::E procData(
						AosRundata *rdata_raw,
						AosDataRecordObj **input_records,
						AosDataRecordObj **output_records);

	virtual bool finish(
			const vector<AosDataProcObjPtr> &procs,
			const AosRundataPtr &rdata);
private:
	bool	config(
				const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);

	bool 	initShuffleInfo(const AosXmlTagPtr &conf);
	
	bool 	configStatKeyField(
			const AosXmlTagPtr &conf,
			OmnString &key_output_rcdname);

	bool 	configStatTimeField(
			const AosXmlTagPtr &conf,
			OmnString &time_output_rcdname);

	bool 	configStatTimesField(
			const AosXmlTagPtr &conf,
			OmnString &time_output_rcdname);

	bool 	configStatValuesField(
			const AosXmlTagPtr &conf,
			OmnString &value_output_rcdname);

	bool 	configStatDocidField(
			const AosXmlTagPtr &conf,
			OmnString &id_output_rcdname);

	bool 	configStatCubeIdField(
			const AosXmlTagPtr &conf,
			OmnString &id_output_rcdname);

	int 	getCubeIdByStatKey(const OmnString &stat_key);

};

#endif
