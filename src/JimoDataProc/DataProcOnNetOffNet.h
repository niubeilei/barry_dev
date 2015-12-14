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
// 12/13/2015 Created by Barry
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProcOnNetOffNet_h
#define Aos_DataProc_DataProcOnNetOffNet_h

#include "StreamEngine/StreamDataProc.h"

class AosDataProcOnNetOffNet : public AosStreamDataProc
{
private:
	AosExprObjPtr				mUserIdExpr;
	AosExprObj*					mRawUserIdExpr;

	AosExprObjPtr				mTimeExpr;
	AosExprObj*					mRawTimeExpr;

	OmnString					mTimeUnit;
	u64							mShreshold;
	AosDateTime					mFirstDay;
	AosDateTime					mCrtDay;
	AosDateTime					mLastDay;

	AosDataRecordObjPtr 		mOnNetOutputRecord;
	AosDataRecordObj*			mRawOnNetOutputRecord;

	AosDataRecordObjPtr 		mOffNetOutputRecord;
	AosDataRecordObj*			mRawOffNetOutputRecord;
	AosXmlTagPtr				mDatasetConf;

public:
	AosDataProcOnNetOffNet(const int ver);
	AosDataProcOnNetOffNet(const AosDataProcOnNetOffNet &proc);
	~AosDataProcOnNetOffNet();

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
	virtual AosDataProcObjPtr cloneProc();

private:
	bool	config(const AosXmlTagPtr &def,
				const AosRundataPtr &rdata);

	int     getMaxThreads() const {return 1;}

	bool 	createOutput(
				const OmnString &dp_name,
				const JSONValue &json_conf,
				const AosRundataPtr &rdata);
	bool 	createOnNetOutput(
				const OmnString &dp_name,
				const JSONValue &json_conf,
				const AosRundataPtr &rdata);
	bool 	createOffNetOutput(
				const OmnString &dp_name,
				const JSONValue &json_conf,
				const AosRundataPtr &rdata);

	bool			runJoin(AosRundata *rdata);

	bool			getNextKey(
						OmnString &key,
						OmnString &day,
						AosDataRecordObj *input_record,
						const AosRundataPtr &rdata);

	bool			createMap(
						map<OmnString, int> &crt_map,
						const AosRundataPtr &rdata);
	bool			createSet(
						const OmnString &crt_day,
						set<OmnString> &crt_set,
						const AosRundataPtr &rdata);
	bool			addSet(
						map<OmnString, int> &crt_map,
						set<OmnString> &crt_set,
						const AosRundataPtr &rdata);
	bool			removeSet(
						map<OmnString, int> &crt_map,
						set<OmnString> &crt_set,
						const AosRundataPtr &rdata);

	bool			appendOnNetEntry(
						const OmnString &key,
						const OmnString&day,
						const AosRundataPtr &rdata);
	bool			appendOffNetEntry(
						const OmnString &key,
						const OmnString &day,
						const AosRundataPtr &rdata);
	bool 			scanFinished(
						const AosDatasetObjPtr &dataset,
						const AosRundataPtr &rdata);
};
#endif
