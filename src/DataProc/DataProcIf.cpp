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
// 2013/12/23 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DataProc/DataProcIf.h"

#include "API/AosApi.h"


AosDataProcIf::AosDataProcIf(const bool flag)
:
AosDataProc(AOSDATAPROC_IF, AosDataProcId::eIf, flag)
{
}


AosDataProcIf::AosDataProcIf(const AosDataProcIf &proc)
:
AosDataProc(proc)
{
}


AosDataProcIf::~AosDataProcIf()
{
}


AosDataProcStatus::E 
AosDataProcIf::procData(
		const AosDataRecordObjPtr &record,
		const u64 &docid,
		const AosDataRecordObjPtr &output,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return AosDataProcStatus::eError;
}


bool
AosDataProcIf::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


AosDataProcObjPtr
AosDataProcIf::cloneProc()
{
	return OmnNew AosDataProcIf(*this);
}


AosDataProcObjPtr
AosDataProcIf::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcIf * proc = OmnNew AosDataProcIf(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}


bool
AosDataProcIf::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// Its format is:
	// 	<dataproc proc_id="convertdoc">
	// 		<fields>
	//			<field zky_input_field_name="xxx.xxx" zky_sep="xxx" />
	//			...
	//		</fields>
	// 	</dataproc>
	aos_assert_r(def, false);
	aos_assert_r(isVersion1(def), false);
	
	AosXmlTagPtr filter = def->getFirstChild(AOSTAG_FILTER);
	aos_assert_r(filter, false);

	mFilter = AosConditionObj::getConditionStatic(filter, rdata);
	aos_assert_r(mFilter, false);

	AosXmlTagPtr true_procs = def->getFirstChild("true_procs");
	aos_assert_r(true_procs, false);

	AosXmlTagPtr false_procs = def->getFirstChild("false_procs");
	aos_assert_r(false_procs, false);

	mTrueProcs.clear();
	mFalseProcs.clear();

	AosDataProcObjPtr proc;
	AosXmlTagPtr proc_conf = true_procs->getFirstChild(true);
	while (proc_conf)
	{
		proc_conf->setAttr("version", 1);
		proc = createDataProc(proc_conf, rdata);
		aos_assert_r(proc, false);

		proc->setTaskDocid(mTaskDocid);
		mTrueProcs.push_back(proc);

		proc_conf = true_procs->getNextChild();
	}

	proc_conf = false_procs->getFirstChild(true);
	while (proc_conf)
	{
		proc_conf->setAttr("version", 1);
		proc = createDataProc(proc_conf, rdata);
		aos_assert_r(proc, false);

		proc->setTaskDocid(mTaskDocid);
		mFalseProcs.push_back(proc);

		proc_conf = false_procs->getNextChild();
	}

	return true;
}


bool
AosDataProcIf::getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos)
{
	bool rslt;
	for (u32 i=0; i<mTrueProcs.size(); i++)
	{
		rslt = mTrueProcs[i]->getRecordFieldInfosRef(rf_infos);
		aos_assert_r(rslt, false);
	}

	for (u32 i=0; i<mFalseProcs.size(); i++)
	{
		rslt = mFalseProcs[i]->getRecordFieldInfosRef(rf_infos);
		aos_assert_r(rslt, false);
	}

	return true;
}


AosDataProcStatus::E
AosDataProcIf::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	aos_assert_r(mFilter, AosDataProcStatus::eError);

	AosDataRecordObj * input_record = input_records[0];
	aos_assert_r(input_record, AosDataProcStatus::eError);

	AosDataRecordObjPtr record = input_record;
	vector<AosDataProcObjPtr> * procs = &mTrueProcs;

	bool rslt = mFilter->evalCond(record, rdata_raw);
	if (!rslt)
	{
		procs = &mFalseProcs;
	}

	AosDataProcStatus::E status;
	for (u32 i=0; i<(*procs).size(); i++)
	{
		status = (*procs)[i]->procData(rdata_raw, input_records, output_records);
		switch (status)
		{
		case AosDataProcStatus::eContinue:
			 break;
		
		default:
			 return status;
		}
	}

	return AosDataProcStatus::eContinue;
}

