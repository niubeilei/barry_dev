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
// 2013/05/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_DataProcObj_h
#define Aos_SEInterfaces_DataProcObj_h

#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DLLObj.h"
#include "SEInterfaces/DataProcId.h"
#include "SEInterfaces/DataProcStatus.h"
#include "SEInterfaces/RecordFieldInfo.h"
#include "SEInterfaces/DataAssemblerObj.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "AosConf/Output.h"
#include <vector>
#include <map>
using namespace std;
using namespace AosConf;

class AosDataProcObj : virtual public OmnRCObject, public AosTaskReporter
{
private:
	static AosDataProcObjPtr smObject;

public:
	~AosDataProcObj() {};
	static void setDataProc(const AosDataProcObjPtr &obj) {smObject = obj;}
	static AosDataProcObjPtr getDataProc() {return smObject;}
	static AosDataProcObjPtr createDataProcStatic(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata);
	
	// Ketty 2014/01/14
	static AosDataProcObjPtr createDataProcV1Static(
						const AosRundataPtr &rdata,
						const AosDataEngineObjPtr &engine,
						const AosXmlTagPtr &def);

	virtual bool config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata) = 0;

	// DataProc interface
	virtual AosDataProcId::E getId() const = 0;
	virtual OmnString getType() const = 0;
	virtual bool needConvert() const = 0;
	virtual bool needDocid() const = 0;
	virtual int getMaxThreads() const = 0;
	virtual OmnString getDataColId() const = 0;
	virtual AosDataProcObjPtr cloneProc() = 0;
	virtual AosDataProcStatus::E procData(
						const AosDataRecordObjPtr &record,
						const u64 &docid,
						const AosDataRecordObjPtr &output, 
						const AosRundataPtr &rdata) = 0;
	virtual bool resolveDataProc(
	 					map<OmnString, AosDataAssemblerObjPtr> &asms,
	 					const AosDataRecordObjPtr &record,
	 					const AosRundataPtr &rdata) = 0;
	virtual AosDataProcObjPtr createDataProc(
						const AosXmlTagPtr &def,
						const AosRundataPtr &rdata) = 0;
	virtual AosDataProcObjPtr create(
			            const AosXmlTagPtr &def,
						const AosRundataPtr &rdata) = 0;
	// Ketty 2013/12/19
	//virtual bool resolveDataProc(
	//			const AosRundataPtr &rdata,
	//			const AosDataEngineObjPtr &data_engine);
	
	virtual void setInputDataRecords(vector<AosDataRecordObjPtr> &records) = 0;
	virtual bool getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos) = 0;
	virtual AosDataProcStatus::E procData(
						AosRundata *rdata_raw,
						AosDataRecordObj **input_records,
						AosDataRecordObj **output_records) = 0;

	virtual AosDataProcStatus::E procData(
						AosRundata* rdata_raw,
						const AosRecordsetObjPtr &lhs_recordset,
						const AosRecordsetObjPtr & rhs_recordset,
						AosDataRecordObj** output_records) = 0;
	
	// Chen Ding, 2013/12/15
	virtual bool resolveDataProc(const AosRundataPtr &rdata) = 0;

	virtual bool start(const AosRundataPtr &rdata) = 0;
	virtual bool finish(const AosRundataPtr &rdata) = 0;
	virtual bool finish(const vector<AosDataProcObjPtr> &procs, const AosRundataPtr &rdata) = 0;
	virtual i64  getProcDataCount() = 0;
	virtual i64  getProcOutputCount() = 0;
	virtual void setPipEnd(bool flag) = 0;

	virtual OmnString getDataProcName() = 0;
	
	// feliica, 2014/04/08
	virtual void setTaskDocid(const u64 task_docid) = 0;
	virtual void onThreadInit(const AosRundataPtr &, void **data){};
	virtual void onThreadExit(const AosRundataPtr &, void **data){};

	//virtual vector<AosDataAssemblerObjPtr> getOutputs() = 0;
	virtual vector<boost::shared_ptr<Output> > getOutputs() = 0;

	virtual bool getFields(vector<RecordFieldInfo*> &lhs, vector<RecordFieldInfo*> &rhs){ return true; }

	// Chen Ding, 2015/01/24
	virtual bool createOutputRecords(AosRundata *rdata) = 0;

	virtual void callback(const bool svr_death) = 0;
	virtual void callback(const AosBuffPtr &resp, const bool svr_death) = 0;

	virtual void setIsStreaming(const bool isStreaming) = 0;
};

#endif

