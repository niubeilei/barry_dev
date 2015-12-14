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
// This data proc is a template to create statistics.
// A stat template has one or more Stat Entry Creators. All stat entry
// creators are implemented as DLLs. When it starts, it retrieves the
// creators. If it fails retrieving any of the creators, it fails the 
// operation.
//
// Modification History:
// 2013/05/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataProc/DataProcStatDyn.h"

#include "SEInterfaces/EntryCreatorObj.h"



AosDataProcStatDyn::AosDataProcStatDyn(const bool flag)
:
AosDataProc(AOSDATAPROC_DLLPROC, AosDataProcId::eDLLProc, flag)
{
}

	
AosDataProcStatDyn::AosDataProcStatDyn(const AosDataProcStatDyn &proc)
:
AosDataProc(proc)
{
}

AosDataProcStatDyn::~AosDataProcStatDyn()
{
}


bool
AosDataProcStatDyn::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// Its configuration is in the form:
	// 	<config ...>
	// 		...
	// 		<creator .../>
	// 		...
	// 	</config>
	aos_assert_r(def, false);

	bool rslt = AosDataProc::config(def, rdata);
	aos_assert_rr(rslt, rdata, false);

	// Create the creator
	AosXmlTagPtr tag = def->getFirstChild("creator");
	aos_assert_rr(tag, rdata, false);
	mCreator = AosEntryCreatorObj::createEntryCreatorStatic(rdata, tag);
	aos_assert_rr(mCreator, rdata, false);

	return true;
}


AosDataProcStatus::E 
AosDataProcStatDyn::procData(
		const AosDataRecordObjPtr &record,			
		const u64 &docid,
		const AosDataRecordObjPtr &output,			
		const AosRundataPtr &rdata)
{
	// This function reads in a record pointed to by 'record', 
	// creates the IILs as needed, and then creates a new record
	// based on 'target_formatter'. The new record is appended 
	// to 'buff'.
	
	aos_assert_rr(record, rdata, AosDataProcStatus::eError);
	aos_assert_rr(output, rdata, AosDataProcStatus::eError);

	// Each creator takes a record and generates zero or one entry.
	// If an entry is generated, it sets the contents to 'output'.
	if (mCreator->run(rdata, record, docid, output)) return AosDataProcStatus::eContinue;
	return AosDataProcStatus::eError;
}

AosDataProcStatus::E
AosDataProcStatDyn::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	// Ketty 2013/12/27
	OmnNotImplementedYet;
	return AosDataProcStatus::eError;
}

AosDataProcObjPtr
AosDataProcStatDyn::cloneProc()
{
	return OmnNew AosDataProcStatDyn(*this);
}


AosDataProcObjPtr
AosDataProcStatDyn::create(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	AosDataProcStatDyn * proc = OmnNew AosDataProcStatDyn(false);
	bool rslt = proc->config(def, rdata);
	aos_assert_r(rslt, 0);
	return proc;
}


bool
AosDataProcStatDyn::resolveDataProc(
		map<OmnString, AosDataAssemblerObjPtr> &asms,
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	/*
	bool rslt = AosDataProc::resolveIILAssembler(asms, record, rdata);
	aos_assert_r(rslt, false);
	
	aos_assert_r(record, false);
	
	mKeyFieldIdx = record->getFieldIdx(mKeyFieldName, rdata);
	aos_assert_r(mKeyFieldIdx >= 0, false);
	
	return true;
	*/
	return true;
}
	
bool
AosDataProcStatDyn::resolveDataProc(
		const AosRundataPtr &rdata,
		const AosDataRecordObjPtr &input_record,
		const AosDataRecordObjPtr &output_record)
{
	/*
	bool rslt = AosDataProc::resolveIILAssembler(asms, record, rdata);
	aos_assert_r(rslt, false);
	
	aos_assert_r(record, false);
	
	mKeyFieldIdx = record->getFieldIdx(mKeyFieldName, rdata);
	aos_assert_r(mKeyFieldIdx >= 0, false);
	
	return true;
	*/
	return true;
}

