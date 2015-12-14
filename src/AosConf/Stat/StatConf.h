////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 04/25/2014 Created by Young
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_AosConf_StatConf_h
#define Aos_AosConf_StatConf_h


#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "XmlUtil/XmlTag.h"
#include "Util/StrSplit.h"
#include "AosConf/DataSet.h"   
#include "AosConf/JobTask.h"
#include "AosConf/ReduceTaskDataProc.h"
#include "AosConf/DataProcStatDoc.h"
#include "SEUtil/JqlTypes.h"

#include <map>
#include <vector>

namespace AosConf
{

struct StatTime
{
	OmnString	mName;
	OmnString	mToFormat;
};


struct Measure{
	OmnString 	 	agr_type;
	OmnString	 	field_name;
	OmnString	 	field_type;
	OmnString		name;
	OmnString		grpby_time_unit;	// Ketty 2014/10/10	
	int				stat_time_idx;		// Ketty 2014/10/13
};

struct Aggregation
{
	OmnString		agr_type;
	OmnString		agr_fun;
	int				agr_pos;
};

class StatConf
{

	AosXmlTagPtr 			mInputDataset;
	AosXmlTagPtr 			mStatDoc;
	AosXmlTagPtr 			mTableDoc;
	AosXmlTagPtr 			mVector2d;
	//AosXmlTagPtr 			mVector2dControl;
	OmnString 	 			mTableName;
	OmnString 	 			mStatDocName;
	OmnString				mStatisticDocObjid;
	OmnString				mStatIdentifyKey;
	OmnString 	 			mStatFiled;
	OmnString 	 			mValueFiled;
	OmnString				mFileDefName;
	OmnString				mTask2FileDefName;
	vector <OmnString> 		mStatFields;
	vector <OmnString> 		mDistinctFields;
	boost::shared_ptr<DataSet> 	mOdsOne;
	boost::shared_ptr<DataSet> 	mOdsInputAll;
	boost::shared_ptr<DataSet> 	mOdsAll;
	boost::shared_ptr<DataSet> 	mDistinctOdsOne;
	boost::shared_ptr<DataSet> 	mDistinctOdsAll;
	boost::shared_ptr<DataSet> 	mOdsStatDocData;
	boost::shared_ptr<DataSet> 	mOdsNewValue;
	OmnString				mOdsOneDatasetName;
	OmnString				mOdsAllDatasetName;
	OmnString				mOdsInputAllDatasetName;
	OmnString				mOutputRcdNameOne;
	OmnString				mOutputRcdNameAll;
	OmnString				mInputRcdNameAll;
	OmnString				mDistinctOdsOneDatasetName;
	OmnString				mDistinctOdsAllDatasetName;
	OmnString				mDistinctOutputRcdNameOne;
	OmnString				mDistinctOutputRcdNameAll;
	OmnString				mOutputIILNameAll;
	OmnString				mInternalId;
	OmnString				mDistinctInternalId;
	OmnString				mStatDocDataDsName;
	OmnString				mStatDocDataRcdName;
	OmnString				mNewValueDsName;
	OmnString				mNewValueRcdName;
	int						mMaxLen;
	int						mDistinctMaxLen;
	int						mStatDocDataRcdLen;
	map<string, int>		mFieldLenMap;
	map<OmnString, AosXmlTagPtr>		mTableFields;
//	int						mJobNum;
	AosXmlTagPtr			mAgrTypeNode;
	//OmnString				mInputValueFieldName;
	OmnString 				mInputTimeFieldName;
	OmnString				mTimeFromField;
	OmnString				mTimeFromFormat;
	OmnString				mTimeToFormat;
	//OmnString 				mAgrType;
	map<OmnString, OmnString> 		mStatTypes;
	//map<int, AosXmlTagPtr>	mStatisticsDoc;
	vector<AosXmlTagPtr>	mStatisticsDoc;
	OmnString				mStatConfLevel;
	OmnString				mStartTimeSlot;
	vector<Measure>			mMeasures;
	bool					mHasCount;
	bool					mHasDistinct;
	bool					mIsDistinct;
	OmnString				mStatDocDataFields;
	OmnString				mDistinctOutputIILNameAll;
	OmnString				mDistinctInputRcdNameOne;
	OmnString				mDistinctInputOneDatasetName;
	boost::shared_ptr<DataSet> 	mDistinctOdsInputOne;

	vector<boost::shared_ptr<AosConf::DataField> > mInputFields;
	vector<boost::shared_ptr<AosConf::DataSet> > mOutputds;
	vector<boost::shared_ptr<AosConf::DataProc> > mMapDataProcs;
	vector<boost::shared_ptr<ReduceTaskDataProc> > mReduceDataProcs;
	vector<boost::shared_ptr<AosConf::JobTask> > mTasks;

	AosXmlTagPtr	  mStatRunDoc;		// Ketty 2014/10/11
	OmnString		  mStatRunDocName;	// Ketty 2014/10/11
	OmnString		  mStatName;		// Ketty 2014/10/10
	vector<StatTime>  mStatTimes;		// Ketty 2014/10/10
	OmnString		  mDftTimeFieldName;	// Ketty 2014/10/10
	OmnString		  mCondText;  		//felicia, 2014/11/19, for condition
	
	boost::shared_ptr<DataRecord> mTask1ReduceDataRecord;	// Ketty 2014/10/10 
	boost::shared_ptr<DataRecord> mTask2ReduceDataRecord;	// Ketty 2014/10/10 

	JQLTypes::OpType 							mOp;

public:
	StatConf(map<OmnString, AosXmlTagPtr> &table_fields); // Ketty 2014/10/10
	StatConf(
		const AosXmlTagPtr &inputds,
		const AosXmlTagPtr &statdoc,
		map<OmnString, AosXmlTagPtr> &table_fields);
	~StatConf();

	// Ketty 2014/10/10
	bool config2(const AosXmlTagPtr &input_dataset_conf,
				const AosXmlTagPtr &run_stat_conf);

	bool config(const AosXmlTagPtr &xml);
	string createConfig(
			OmnString &objid, 
			OmnString &ctnr_objid,
			const u32 &max_thread);

	bool	createConfig();

	vector<boost::shared_ptr<AosConf::DataField> > & getInputFields()
	{
		return mInputFields;
	}
	
	vector<boost::shared_ptr<AosConf::DataSet> >& getOutputDataset()
	{
		return mOutputds;
	}
	
	vector<boost::shared_ptr<AosConf::DataProc> > & getMapTaskDataProc()
	{
		return mMapDataProcs;
	}
	
	vector<boost::shared_ptr<ReduceTaskDataProc> > & getReduceTaskDataProc()
	{
		return mReduceDataProcs;
	}
	
	vector<boost::shared_ptr<AosConf::JobTask> > & getTasks()
	{
		return mTasks;
	}
	

	boost::shared_ptr<JobTask> createConfig0();
	boost::shared_ptr<JobTask> createConfig1();
	boost::shared_ptr<JobTask> createConfig2();
	boost::shared_ptr<JobTask> createConfig3();
	boost::shared_ptr<JobTask> createConfig4();
	boost::shared_ptr<JobTask> createConfig5();
	
	boost::shared_ptr<JobTask> createDistinctConfig1();
	boost::shared_ptr<JobTask> createDistinctConfig2();
	/*
	bool createConfig();
	bool createConfig1();
	bool createConfig2();
	bool createConfig3();
	bool createConfig4();
	bool createConfig5();
	*/

	// Ketty 2014/10/10
	bool 	configNew(
				const AosXmlTagPtr &input_dataset_conf,
				const AosXmlTagPtr &run_stat_conf,
				map<OmnString, AosXmlTagPtr> &table_fields);

	boost::shared_ptr<JobTask> createConfig0New();
	boost::shared_ptr<JobTask> createConfig2New();
	boost::shared_ptr<JobTask> createConfig4New();

	virtual void setOp(JQLTypes::OpType op) { mOp = op; }

private:
	//Phil 2015/01/15
	bool isMeasureField(OmnString name);

	// Ketty 2014/10/10
	bool	configCondition(const AosXmlTagPtr &run_stat_conf);

	bool 	configStatKeyFields(const AosXmlTagPtr &run_stat_conf, map<OmnString, AosXmlTagPtr> &table_fields);

	int 	calculateKeyFieldLen(
				//const AosXmlTagPtr &datafields_tag,
				map<OmnString, AosXmlTagPtr> &table_fields,
				const OmnString &key_field_name,
				const OmnString &key_field_type);

	bool 	configTimeField(const AosXmlTagPtr &run_stat_conf);

	bool 	configMeasures(const AosXmlTagPtr &run_stat_conf);

	OmnString convertTimeUnitToTimeFormat(const OmnString &time_unit_str);
	
	bool 	createTask1InputDataRecord(boost::shared_ptr<DataRecord> &inputrcd);
	bool 	addTask1TimeDataField(boost::shared_ptr<DataRecord> &inputrcd);

	bool 	createTask1MapTask(
				boost::shared_ptr<MapTask> &mtask,
				boost::shared_ptr<DataRecord> &inputrcd,
				const OmnString &asm_recordname);

	bool 	createTask1ReduceTask(
				boost::shared_ptr<ReduceTask> &rtask,
				const OmnString &ds_name2,
				const OmnString &asm_recordname);

	bool 	addTask1InputDataRecordNoKey(boost::shared_ptr<DataRecord> &inputrcd);

	bool 	createTask1MapTaskNoKey(
				boost::shared_ptr<MapTask> &mtask,
				const OmnString &inputrecordname,
				const OmnString &asm_recordname);

	bool 	createTask1ReduceTaskNoKey(
				boost::shared_ptr<ReduceTask> &rtask,
				const OmnString &ds_name2,
				const OmnString &asm_recordname);

	bool 	createTask2LhsInputDataset(
				boost::shared_ptr<DataSet> &lhs_inputds,
				const OmnString &lhs_input_dsname,
				const OmnString &lhs_inputrcd_name);

	bool 	createTask2RhsInputDataset(
				boost::shared_ptr<DataSet> &rhs_inputds,
				const OmnString &rhs_input_dsname,
				const OmnString &rhs_inputrcd_name);

	bool 	createTask2OutputDataset(
				boost::shared_ptr<JobTask> &jobtask,
				const OmnString &output_all_dsname,
				vector<OmnString> &outputds_names,
				const OmnString &output_dsname);

	bool 	createTask2ReduceTask(
				boost::shared_ptr<ReduceTask> &rtask,
				const OmnString &output_all_rcdname,
				const OmnString &output_all_dsname,
				vector<OmnString> &outputds_names,
				vector<OmnString> &outputrcd_names,
				vector<OmnString> &outputiil_names,
				vector<OmnString> &outputproc_names,
				vector<OmnString> &outputcol_names,
				const OmnString &df_rcd_name,
				const OmnString &output_dsname);

	bool 	createTask2MapTask(
				boost::shared_ptr<MapTask> &mtask,
				const OmnString &lhs_input_dsname,
				const OmnString &lhs_inputrcd_name,
				const OmnString &rhs_input_dsname,
				const OmnString &rhs_inputrcd_name,
				const OmnString &df_rcd_name,
				const OmnString &output_all_rcdname,
				vector<OmnString> &outputrcd_names);

	bool 	createTask3InputDataset(
				boost::shared_ptr<DataSet> &lhs_inputds,
				const OmnString &lhs_inputrcd_name);

	bool 	createTask3DataProcStatDoc(
				boost::shared_ptr<DataProcStatDoc> &dpc,
				const OmnString &input_fieldname_all);

	bool 	createTask3InputDataRecord(
				boost::shared_ptr<DataRecord> &input_rcd,
				const OmnString &lhs_inputrcd_name);

};
}



#endif



