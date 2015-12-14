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
#include "DataProc/DataProc.h"

#include "API/AosApi.h"
#include "Job/Job.h"
#include "DataProc/DataProcCompose.h"
#include "DataProc/DataProcCount.h"
#include "DataProc/DataProcField.h"
#include "DataProc/DataProcFilter.h"
#include "DataProc/DataProcRange.h"
#include "DataProc/DataProcTimed.h"
#include "DataProc/DataProcIILIndexing.h"
#include "DataProc/DataProcStat.h"
#include "DataProc/DataProcStatComp.h"
#include "DataProc/DataProcConvertDoc.h"
#include "DataProc/DataProcJoin.h"
#include "DataProc/DataProcStatIndex.h"
#include "DataProc/DataProcStatDocShuffle.h"
#include "DataProc/DataProcIf.h"
#include "DataProc/DataProcCarNum.h"

#include "SEInterfaces/DLLDataProcObj.h"
#include "JimoProg/JimoProgGloble.h"
#include "Thread/Mutex.h"

#include "JimoAPI/JimoParserAPI.h"


static AosDataProcObjPtr 	sgProcs[AosDataProcId::eMax];
static OmnMutex				sgLock;
static OmnMutex				sgInitLock;
static bool					sgInited = false;


AosDataProc::AosDataProc(
		const OmnString &type, 
		const AosDataProcId::E id, 
		const bool flag)
:
mId(id),
mType(type),
mNeedConvert(false),
mNeedDocid(false)
{
	mIsStreaming = false;
	if (flag) registerProc(type, this);
	mName = "";
}

AosDataProc::AosDataProc(const AosDataProc &rhs)
:
mId(rhs.mId),
mType(rhs.mType),
mTaskDocid(rhs.mTaskDocid)
{
	mIsStreaming = rhs.mIsStreaming;
	mName = rhs.mName;
	mTargetReporter = rhs.mTargetReporter;
}

AosDataProc::~AosDataProc()
{
	//OmnScreen << "data proc deleted, " << endl;
}


bool
AosDataProc::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	mNeedConvert = def->getAttrBool(AOSTAG_NEED_CONVERT, false);
	mConvertName = def->getAttrStr(AOSTAG_CONVERT_NAME);
	mDataColId = def->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
	
	return true;
}

bool
AosDataProc::init(const AosRundataPtr &rdata)
{
	static AosDataProcCompose			lsCompose(true);
	static AosDataProcCount				lsCount(true);
	static AosDataProcFilter			lsFilter(true);
	static AosDataProcRange				lsRange(true);
	static AosDataProcTimed				lsTimed(true);
	static AosDataProcIILIndexing		lsIILIndexing(true);
	static AosDataProcField				lsField(true);
	static AosDataProcStat				lsStat(true);
	static AosDataProcStatComp			lsStatComp(true);
	static AosDataProcConvertDoc		lsConvertDoc(true);
	//static AosDataProcJoin				lsJoin(true);
	static AosDataProcStatIndex			lsStatIndex(true);
	//static AosDataProcStatDoc			lsStatDoc(true);
	//static AosDataProcUnicomDocument	lsUnicomDocument(true);
	static AosDataProcStatDocShuffle	lsStatDocShuffle(true);
	static AosDataProcIf				lsIf(true);
	//static AosDataProcWordParser		lsWordParser(true);
	static AosDataProcCarNum			lsCarNum(true);

	sgInitLock.lock();
	sgInited = true;
	AosDataProcId::check();
	sgInitLock.unlock();
	return true;
}


AosDataProcObjPtr 
AosDataProc::createDataProc(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	if (!sgInited) init(rdata);

	aos_assert_r(def, 0);
	
	if (def->getAttrStr("type") == "jimo_parser")
	{
		AosJimoParserObjPtr jimo_parser = Jimo::jimoCreateJimoParser(rdata.getPtr());
		aos_assert_rr(jimo_parser, rdata, 0);

		AosJimoProgObj *prog = NULL;
		vector<AosJqlStatementPtr> statements;
		OmnString stmt = def->getNodeText();
		bool rslt = jimo_parser->parse(rdata.getPtr(), prog, stmt, statements);
		aos_assert_rr(rslt, rdata, 0);
		if (statements.size() != 1)
		{
			    AosLogError(rdata, true, "failed_parsing_dataproc_statement")
					        << AosFN("Statement") << stmt << enderr;
				    return NULL;
		}

		OmnString ss;
		AosJimoProgObjPtr jimo_prog = OmnNew AosJimoProgGloble(); 
		statements[0]->setContentFormat(OmnString("print"));
		//statements[0]->run(rdata.getPtr(), jimo_prog.getPtr(), ss, false);

		AosDataProc *proc = dynamic_cast<AosDataProc*>(statements[0].getPtr());
		if (!proc)
		{
			AosSetErrorUser(rdata, "internal_error") << enderr;
			return 0;
		}
		return proc;
	}

	// felicia, 2014/02/24
	//AosDataProcId::E id = AosDataProcId::toEnum(def->getAttrStr("proc_id"));
	AosDataProcId::E id = AosDataProcId::toEnum(def->getAttrStr("type"));

	// Chen Ding, 2013/07/20
	if (!AosDataProcId::isValid(id))
	{
		// Check whether it is a dynamic data proc. It assumes 'def':
		// 	<def jimo_objid="xxx" .../>
		AosJimoPtr jimo = AosCreateJimo(rdata, def);
		if (!jimo)
		{
			AosSetErrorUser(rdata, "dataproc_not_defined")
				<< ". " << AOSDICTERM("worker_doc", rdata.getPtr()) << ": " 
				<< def->toString() << enderr;
			return 0;
		}

		// AosJimoType::E type = jimo->getType();
		// if (type != AosJimoType::eDataProc)
		// {
		// 	AosSetErrorUser(rdata, "not_dataproc")
		// 		<< ". " << AOSDICTERM("worker_doc", rdata) << ": " 
		// 		<< def->toString() << enderr;
		// 	return 0;
		// }

		//AosDataProc *proc = dynamic_cast<AosDataProc*>(jimo.getPtr());
		AosDataProc *proc = dynamic_cast<AosDataProc*>(jimo.getPtr());
		if (!proc)
		{
			AosSetErrorUser(rdata, "internal_error") << enderr;
			return 0;
		}
		//proc->config(def, rdata);
		return proc;
	}

	// Chen Ding, 2013/05/04
	// AosDataProcObjPtr proc;
	// if (id == AosDataProcId::eDLLProc)
	// {
	// 	// It is a DLL proc. 
	// 	// rdata->setDLLObj(0);
	// 	AosDLLObjPtr dllobj = AosCreateDLLObj(rdata, def);
	// 	aos_assert_rr(dllobj, rdata, 0);
	// 	proc = dllobj->convertToDataProc(rdata);
	// 	aos_assert_rr(proc, rdata, 0);
	// }
	// else
	// {
	AosDataProcObjPtr proc = sgProcs[id]->cloneProc();
	// }

	aos_assert_r(proc, 0);
	return proc->create(def, rdata);
}


bool
AosDataProc::registerProc(const OmnString &name, AosDataProc *proc)
{
	sgLock.lock();
	if (!AosDataProcId::isValid(proc->mId))
	{
		sgLock.unlock();
		OmnString errmsg = "Incorrect data proc id: ";
		errmsg << proc->mId;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgProcs[proc->mId])
	{
		sgLock.unlock();
		OmnString errmsg = "Proc already registered: ";
		errmsg << proc->mId;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgProcs[proc->mId] = proc;
	sgProcs[proc->mId].setDelFlag(false);
	bool rslt = AosDataProcId::addName(name, proc->mId);
	sgLock.unlock();
	return rslt;
}


bool
AosDataProc::resolveIILAssembler(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	if(mDataColId != "")
	{
		map<OmnString, AosDataAssemblerObjPtr>::iterator itr;
		itr = asms.find(mDataColId);
		if(itr != asms.end())
		{
			mIILAssembler = itr->second;
		}
	}

	if (!mIILAssembler && !mNeedConvert)
	{
		AosSetErrorU(rdata, "missing_iil_asm:") << mDataColId;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	return true;
}
	

u64
AosDataProc::getFieldU64Value(
		AosRundata *rdata_raw,
		AosDataRecordObj *record,
		const u32 field_idx)
{
	// Ketty 2013/12/19
	AosValueRslt key_rslt;
	bool rslt = record->getFieldValue(field_idx, key_rslt, false, rdata_raw);
	aos_assert_r(rslt, 0);
	
	u64 value = 0;
	rslt = key_rslt.getU64();
	aos_assert_r(rslt, 0);
	return value;
}

bool
AosDataProc::resolveDataProc(const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}

//bool
//AosDataProc::resolveDataProc(
//		const AosRundataPtr &rdata,
//		const AosDataEngineObjPtr &data_engine)
//{
//	// Ketty 2013/12/19
//	OmnShouldNeverComeHere;
//	return false;
//}

bool 
AosDataProc::getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos)
{
	// Ketty 2013/12/19
	OmnShouldNeverComeHere;
	return false;
}


void 
AosDataProc::setInputDataRecords(vector<AosDataRecordObjPtr> &records)
{
}  


void 
AosDataProc::setPipEnd(bool flag)
{
	OmnShouldNeverComeHere;
}


AosDataProcStatus::E
AosDataProc::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	// Ketty 2013/12/19
	OmnShouldNeverComeHere;
	return AosDataProcStatus::eError;
}

AosDataProcStatus::E 
AosDataProc::procData(
			AosRundata* rdata_raw,
			const AosRecordsetObjPtr &lhs_recordset,
			const AosRecordsetObjPtr &rhs_recordset,
			AosDataRecordObj **output_records)
{
	OmnShouldNeverComeHere;
	return AosDataProcStatus::eError;
}

	
bool
AosDataProc::isVersion1(const AosXmlTagPtr &def)
{
	return def->getAttrU64("version", 0) == 1;
}

AosDataRecordObj *
AosDataProc::getRecord(
		RecordFieldInfo & info,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	AosDataRecordObj * record;
	if(info.mIsDataEngineInput)
	{
		record = input_records[info.mRecordIdx];
	}
	else
	{
		record = output_records[info.mRecordIdx];
	}
	aos_assert_r(record, 0);
	return record;
}


bool 
AosDataProc::start(const AosRundataPtr &rdata)
{
	return true;
}


bool 
AosDataProc::finish(const AosRundataPtr &rdata)
{
	return true;
}

bool 
AosDataProc::finish(const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	return true;
}


// Chen Ding, 2015/01/24
bool 
AosDataProc::createOutputRecords(AosRundata *rdata)
{
	OmnShouldNeverComeHere;
	return true;
}


//for log
//barry 2015/11/04
void
AosDataProc::showDataProcInfo(
		const char *file,
		const int line,
		const OmnString &action,
		const OmnString &procType)
{
	OmnString msg;
	msg	<< " DPRT:" << procType << " DPROC:" << mName;
	report(file, line, action, msg);
}


void
AosDataProc::showDataProcInfo(
		const char *file,
		const int line,
		const i64 procNum, 
		const i64 outputNum)
{
	OmnString msg;
	msg << " DataProc: " << mName 
		<< ", proc num : " << procNum 
		<< ", filter num : " << procNum - outputNum
		<< ", output num : " << outputNum;
	report(file, line, "show", msg);
}

