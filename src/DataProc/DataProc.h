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
// 04/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataProc_DataProc_h
#define Aos_DataProc_DataProc_h

#include "alarm_c/alarm.h"
#include "Conds/Condition.h"
#include "Conds/Ptrs.h"
#include "DataProc/Ptrs.h"
#include "DataAssembler/Ptrs.h"
#include "DataAssembler/DataAssembler.h"
#include "ImportData/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataAssemblerObj.h"
#include "SEInterfaces/DataProcStatus.h"
#include "SEInterfaces/DataProcObj.h"
#include "SEInterfaces/DataRecordObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
#include <map>
using namespace std;

class AosDataProc : public AosDataProcObj
{
	OmnDefineRCObject;

public:
	enum Status
	{
		eInvalid,

		eDataTooShort,
		eRecordFiltered,
		eContinue,
		eError
	};
	
	struct Field
	{
		RecordFieldInfo	mInput;	
		RecordFieldInfo	mOutput;
	};

protected:
	AosDataProcId::E		mId;
	OmnString				mType;
	bool					mNeedConvert;
	bool					mNeedDocid;
	bool					mIsStreaming;
	OmnString				mDataColId;
	OmnString				mConvertName;
	AosDataAssemblerObjPtr	mIILAssembler;
	u64						mTaskDocid;

	AosTaskObjPtr			mTask;
	OmnString				mName;
	
public:
	AosDataProc(
			const OmnString &type,
			const AosDataProcId::E id,
			const bool flag);

	AosDataProc(const AosDataProc &rhs);

	~AosDataProc();

	virtual AosDataProcId::E getId() const {return mId;}
	virtual OmnString getType() const {return mType;}
	virtual bool needConvert() const {return mNeedConvert;} 
	virtual bool needDocid() const {return mNeedDocid;}
	virtual OmnString getDataColId() const {return mDataColId;}

	// Chen Ding, 2013/12/15
	virtual bool resolveDataProc(const AosRundataPtr &rdata);

	//virtual bool resolveDataProc(
	 //					map<OmnString, AosDataAssemblerObjPtr> &asms,
 	//					const AosDataRecordObjPtr &record,
 	//					const AosRundataPtr &rdata);
	virtual AosDataProcObjPtr createDataProc(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	// virtual AosDataProcObjPtr create(
	// 		            const AosXmlTagPtr &def,
	// 					const AosRundataPtr &rdata);
	
	// Ketty 2013/12/19
	//virtual bool resolveDataProc(
	//			const AosRundataPtr &rdata,
	//			const AosDataEngineObjPtr &data_engine);
	virtual bool getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos);	
	virtual AosDataProcStatus::E procData(
				AosRundata *rdata_raw,
				AosDataRecordObj **input_records,
				AosDataRecordObj **output_records);

	virtual AosDataProcStatus::E procData(
						AosRundata* rdata_raw,
						const AosRecordsetObjPtr &lhs_recordset,
						const AosRecordsetObjPtr &rhs_recordset,
						AosDataRecordObj** output_records);

	virtual void setTaskDocid(const u64 task_docid){mTaskDocid = task_docid;}

	virtual bool start(const AosRundataPtr &rdata);
	virtual bool finish(const AosRundataPtr &rdata);
	virtual bool finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata);
	virtual OmnString getDataProcName(){OmnAlarm << "error" << enderr; return "";}
	
	virtual bool config(const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
private:
	bool registerProc( 	const OmnString &name,
						AosDataProc *proc);

protected:
	static bool	init(const AosRundataPtr &rdata);

	virtual bool	resolveIILAssembler(
						map<OmnString, AosDataAssemblerObjPtr> &asms,
						const AosDataRecordObjPtr &record,
						const AosRundataPtr &rdata);

	// Ketty 2013/12/27
	u64 	getFieldU64Value(
				AosRundata *rdata_raw,
				AosDataRecordObj *record,
				const u32 field_idx);

	bool	isVersion1(const AosXmlTagPtr &def);
	AosDataRecordObj * getRecord(
				RecordFieldInfo &info,
				AosDataRecordObj **input_records,
				AosDataRecordObj **output_records);

public:
	// Chen Ding, 2015/01/24
	virtual bool createOutputRecords(AosRundata *rdata);

	virtual void setInputDataRecords(vector<AosDataRecordObjPtr> &records);
	virtual void setPipEnd(bool flag);
	virtual i64  getProcDataCount() { return -1; }
	virtual i64  getProcOutputCount() { return -1; }

	//virtual vector<AosDataAssemblerObjPtr> getOutputs() {return vector<AosDataAssemblerObjPtr>();}
	virtual vector<boost::shared_ptr<Output> > getOutputs() {return vector<boost::shared_ptr<Output> >();}
	
	virtual int getMaxThreads() const
	{
		return 2;
	}

	virtual void callback(const bool svr_death){}
	virtual void callback(const AosBuffPtr &resp, const bool svr_death){}
	
	virtual void setIsStreaming(const bool isStreaming){mIsStreaming = isStreaming;}

	void showDataProcInfo(
			const char *file,
			const int line,
			const OmnString &action,
			const OmnString &procType);

	void showDataProcInfo(
			const char *file,
			const int line,
			const i64 procNum, 
				const i64 outputNum);

};
#endif
