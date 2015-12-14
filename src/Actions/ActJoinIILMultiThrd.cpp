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
// IIL joins are over-simplified 'table joins', where IILs are special 
// form of 'tables'. There are only two columns in IILs: Key and Value.
// Two IIls can be joined in a number of ways:
// 		eKey
// 		eValue
// 		eFamily
// 		eMember
// 		eSpecificMember
//
// This action supports only equi-joins. Further, this action assumes the
// matching column (either 'key' or 'value' column) is sorted. For instance, 
// if the matching column is the key-column, the normal IIL is used; if
// the matching column is the value-column, the companion IIL is used. 
//
// This action supports both inner or outer equi-joins. For two IILs: IIL1
// and IIL2, 
// 1. it is an inner join if both is configured to return values only upon matching
// 2. it is a left join if IIL2 is configured to always return a value 
// 3. it is a right join if IIL1 is configured to always return a value
// 4. it is a full join if both are configured to always return a value
//
// Modification History:
// 05/10/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActJoinIILMultiThrd.h"
/*
#include "Actions/ActJoinIILMultiThrdRunner.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataRecord/DataRecord.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "DocProc/DocAssembler.h"
#include "DocProc/DocAssemblerType.h"
#include "DataAssembler/DataAssembler.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/QueryContextObj.h"
#include "SEInterfaces/IILScannerObj.h"
#include "SEInterfaces/Ptrs.h"
#include "SEUtil/GroupbyProc.h"
#include "SEUtil/GroupbyType.h"
#include "Util/OmnNew.h"
#include "Util/ValueRslt.h"
#include "Util/DataTable.h"
#include "Util/TableAssembler.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


AosActJoinIILMultiThrd::AosActJoinIILMultiThrd(const bool flag)
:
AosSdocAction(AOSACTTYPE_JOINIILMULTITHRD, AosActionType::eJoinIILMultiThrd, flag),
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin)),
mPrefixLen(-1),
mRunInMultiThreads(false)
{
}


AosActJoinIILMultiThrd::AosActJoinIILMultiThrd(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_JOINIILMULTITHRD, AosActionType::eJoinIILMultiThrd, false),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
mPrefixLen(-1),
mRunInMultiThreads(false)
{
	if (!config(def, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosActJoinIILMultiThrd::~AosActJoinIILMultiThrd()
{
}


AosActionObjPtr
AosActJoinIILMultiThrd::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActJoinIILMultiThrd(def, rdata);
	}

	catch (const OmnExcept &e)
	{
		rdata->setError() << "Failed to create action: " << e.getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosActJoinIILMultiThrd::config(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// The config is in the form:
	// 	<sdoc 
	// 		AOSTAG_PREFIX_LENGTH="xxx">
	// 		<AOSTAG_IIL_SCANNERS>
	// 			<AOSTAG_IILSCANNER .../>
	// 			<AOSTAG_IILSCANNER .../>
	// 			...
	// 		</AOSTAG_IIL_SCANNERS>
	// 		<AOSTAG_GROUPBYOPR>
	// 			...
	// 		</AOSTAG_GROUPBYOPR>
	// 		<iilassembler .../>
	// 		<docassembler .../>
	// 	</sdoc>
	aos_assert_r(rdata && rdata->getSiteid() > 0, false);
	aos_assert_rr(def, rdata, false);
	
	// Chen Ding, 06/11/2012
	// Retrieve Prefix Length
	mPrefixLen = def->getAttrInt(AOSTAG_PREFIX_LENGTH, -1);
	mRunInMultiThreads = def->getAttrBool(AOSTAG_RUN_IN_MULTITHREADS, false);

	// Retrieve IIL Scanners
	AosXmlTagPtr scanners = def->getFirstChild(AOSTAG_IIL_SCANNERS);
	if (!scanners)
	{
		AosSetErrorU(rdata, "missing_iil_scanners") << ": " << def->toString();
		return false;
	}

	AosXmlTagPtr scanner_tag = scanners->getFirstChild();
	int seqno = 0;
	while (scanner_tag)
	{
		AosIILScannerObjPtr scanner = AosIILScannerObj::createIILScannerStatic(
				seqno++, scanner_tag, rdata);
		aos_assert_rr(scanner, rdata, false);
		mIILScanners.push_back(scanner);
		scanner_tag = scanners->getNextChild();
	}	
	
	if (mIILScanners.size() <= 0)
	{
		AosSetErrorU(rdata, "missing_iil_scanners") << ": " << def->toString();
		return false;
	}
	
	// Create the Record
	AosXmlTagPtr record_tag = def->getFirstChild(AOSTAG_RECORD);
	if (!record_tag)
	{
		AosSetErrorU(rdata, "missing_record_conf") << ": " << def->toString();
		return false;
	}

	mRecord = AosDataRecordObj::createDataRecordStatic(record_tag, rdata);
	if (!mRecord)
	{
		AosSetErrorU(rdata, "missing_record") << ": " << def->toString();
		return false;
	}

	// Create IILAssembler
	AosXmlTagPtr assembler = def->getFirstChild("iilassember");
	if (assembler)
	{
		mIILAssembler = AosDataAssembler::getDataAssembler(
			mTask, assembler, rdata AosMemoryCheckerArgs);
		aos_assert_rr(mIILAssembler, rdata, false);
	}

	// Create DocAssembler
	assembler = def->getFirstChild("docassembler");
	if (assembler)
	{
		// Chen Ding, 06/19/2012
		// Check whether it is a valid tag. 
		AosDocAssemblerType::E type = AosDocAssemblerType::toEnum(
				assembler->getAttrStr(AOSTAG_TYPE));
		if (AosDocAssemblerType:.isNull(type))
		{
			try
			{
				mDocAssembler = OmnNew AosDocAssembler(
					mTask, 0, mRecord, assembler, rdata AosMemoryCheckerArgs);
			}
	
			catch (...)
			{
				OmnAlarm << "Failed creating doc assembler" << enderr;
				return false;
			}
		}
	}

	// Chen Ding, 05/29/2012
	AosXmlTagPtr gbtag = def->getFirstChild(AOSTAG_GROUPBY_PROC);
	if (gbtag)
	{
		// Chen Ding, 06/19/2012
		// Check whether it is a valid tag. 
		AosGroupbyType::E type = AosGroupbyType::toEnum(gbtag->getAttrStr(AOSTAG_TYPE));
		if (AosGroupbyType:.isNull(type))
		{
			try
			{
				mGroupbyProc = OmnNew AosGroupbyProc(gbtag, rdata);
			}

			catch (...)
			{
				AosSetErrorU(rdata, "failed_create_groupbyproc") << ": " << def->toString();
				return false;
			}
		}
	}

	return true;
}


bool 
AosActJoinIILMultiThrd::run(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This task joins two or more IILs to create a new table. The
	// IILs are specified in mIILScanners. 
	// 1. Issue a request to copy the data for all the IILs in mIILScanners.
	// 2. It waits until all responses are received.
	// 3. Once all responses are received, it starts the join process.
	//
	// Join Process:
	// 1. Every IIL has a cursor, which sets to the first entry.
	// 2. Starting from left, it retrieves the current 'Join Key', 
	//    passes it on to the next IIL with a 'Record To be Created'.
	//    If the next IIL accepts the value, it sets its value to 
	//    the record, and passes the 'Join Key' to the next IIL, 
	//    if any. 
	//
	//    If any IIL rejects the 'Join Key', the record is dropped, 
	//    and the control goes back to the first IIL. It moves the cursor
	//    to the next one, if any, and repeats the above process.
	//
	//    If all IILs accept the 'Join Key', the record is created and 
	//    inserted into the table.
	//
	// 3. During the above process, if any IIL consumed all its data but
	//    there are still remaining data to process, it will pause the process,
	//    fetch the data, and then resume the process.
	//
	// This join will creates at most one record for each 'Join Key' 
	// in the first IIL.

	bool rslt;
	if (mIILAssembler)
	{
		rslt = mIILAssembler->sendStart(mRundata);
		if(!rslt)
		{
			actionFailed();
			return false;
		}
	}
	
	if (mDocAssembler)
	{
		rslt = mDocAssembler->sendStart(mRundata);
		if(!rslt)
		{
			actionFailed();
			return false;
		}
	}

	if (mRunInMultiThreads)
	{
		return runInMultiThreads(sdoc, rdata);
	}
	return runInSingleThread(sdoc, rdata);
}


bool
AosActJoinIILMultiThrd::runInSingleThread(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This function creates one runner.
	AosActJoinIILMultiThrdPtr thisptr(this, true);
	OmnThrdShellProcPtr runner = OmnNew AosActJoinIILMultiThrdRunner(thisptr, 
			mIILScanners, rdata);
//	return OmnThreadShellMgr::getSelf()->proc(runner);
}


bool
AosActJoinIILMultiThrd::runInMultiThreads(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	// This function runs the join in multi-threads. Among all the IIL scanners, 
	// there shall be one and only one controlling IIL scanner. The distribution
	// is controlled by that IIL scanner. If no controlling scanner is specified
	// or more than one controlling scanner is specified, it is an error. 
	// It will report the error and run in single thread.
	//
	// If it is running in multi-threads, it uses the controlling thread to 
	// partition the IIL into multiple portions. Each portion runs in a separate
	// thread. It uses the thread shells to run so that the total number of 
	// threads can be controlled by the thread shell manager. 
	//
	// Note that the controlling IIL scanner may have a condition, which specifies
	// a range:
	// 			[min, max]
	// This function asks the controlling IIL scanner to partition the IIL scanner.
	// If the total number of entries is 'too small', it will run in single thread.
	//
	// The number of threads to partition is controlled by the number of CPU cores.
	// It first partition the IIL scanner using NumCpuCores. If each partition is
	// bigger than smMaxSegmentSize, it uses 2*NumCpuCores number of threads. Otherwise,
	// it uses NumCpuCores number of threads.
	
	// 1. Check whether there is one and only one controlling IIL scanner.
	int nn = 0;
	int controlling_idx = -1;
	for (u32 i=0; i<mIILScanners.size(); i++)
	{
		if (mIILScanners[i]->isControllingScanner()) 
		{
			nn++;
			controlling_idx = i;
		}
	}

	if (nn != 0)
	{
		AosSetErrorU(rdata, "incorrect_num_ctrl_scanners") << ": " << nn << enderr;
		return runInSingleThread(sdoc, rdata);
	}

	if (controlling_idx < 0)
	{
		AosSetErrorU(rdata, "internal_error") << enderr;
		return runInSingleThread(sdoc, rdata);
	}

	AosIILScannerObjPtr scanner = mIILScanners[controlling_idx];
	int num_cores = AosGetNumCpuCores();
	if (num_cores <= 0)
	{
		AosSetErrorU(rdata, "missing_cpu_cores") << enderr;
		return runInSingleThread(sdoc, rdata);
	}

	AosBuffPtr buff;
	if (!AosGetIILDistributionMap(mIILScanners[controlling_idx]->getQueryContext(), 
				2*num_cores, buff, rdata))
	{
		AosSetErrorU(rdata, "failed_get_distr_map") << enderr;
		return runInSingleThread(sdoc, rdata);
	}

	// 'buff' is in the form:
	// 	number of segments		u32
	// 	number of entries		u64
	// 	Key						string
	// 	number of entries		u32
	// 	key						string
	// 	number of entries		u32
	// 	...
	u32 num_segments = buff->getU32(0);
	if (num_segments < 2)
	{
		AosSetErrorU(rdata, "internal_error") << enderr;
		return runInSingleThread(sdoc, rdata);
	}

	u64 total_entries = buff->getU64(0);
	if (total_entries <= 0)
	{
		AosSetErrorU(rdata, "internal_error") << enderr;
		return runInSingleThread(sdoc, rdata);
	}

	vector<OmnThrdShellProcPtr> runners;
	if (total_entries / num_cores <= smMaxEntriesPerThread)
	{
		// Run in num_cores threads
		OmnString start_key = buff->getStr("");
		u64 start_docid = buff->getU64(0);
		for (u32 i=1; i<num_segments-1; i+=2)
		{
			buff->getStr("");
			buff->getU64(0);

			OmnString end_key = buff->getStr("");
			u64 end_docid = buff->getU64(0);

			createOneThreadEntry(start_key, start_docid, 
					end_key, end_docid, controlling_idx, runners, rdata);
			
			start_key = end_key;
			start_docid = end_docid;
		}

		createLastThreadEntry(start_key, start_docid, controlling_idx, runners, rdata);
	}
	else
	{
		// Run in 2*num_cores threads
		OmnString start_key = buff->getStr("");
		u64 start_docid = buff->getU64(0);
		for (u32 i=1; i<num_segments; i++)
		{
			OmnString end_key = buff->getStr("");
			u64 end_docid = buff->getU64(0);

			createOneThreadEntry(start_key, start_docid, 
					end_key, end_docid, controlling_idx, runners, rdata);

			start_key = end_key;
			start_docid = end_docid;
		}
	}

	aos_assert_rr(runners.size() > 0, rdata, false);
//	return OmnThreadShellMgr::getSelf()->procSync(runners);
}


bool
AosActJoinIILMultiThrd::createLastThreadEntry(
		const OmnString &start_key,
		const u64 &start_docid,
		const u32 controlling_iilidx, 
		vector<OmnThrdShellProcPtr> &runners,
		const AosRundataPtr &rdata)
{

	// This function creates one thread entry. 
	vector<AosIILScannerObjPtr> scanners;
	for (u32 i=0; i<mIILScanners.size(); i++)
	{
		AosIILScannerObjPtr scanner = mIILScanners[i]->clone();
		if (controlling_iilidx == i)
		{
			scanner->setCondition(start_key, start_docid, rdata);
		}
		else
		{
			scanner->setCondition(start_key, start_docid, rdata);
		}
		scanners.push_back(scanner);
	}

	AosActJoinIILMultiThrdPtr thisptr(this, true);
	OmnThrdShellProcPtr runner = OmnNew AosActJoinIILMultiThrdRunner(thisptr, scanners, rdata);
	runners.push_back(runner);
	
	return true;
}


bool
AosActJoinIILMultiThrd::createOneThreadEntry(
		const OmnString &start_key,
		const u64 &start_docid,
		const OmnString &end_key,
		const u64 &end_docid,
		const u32 controlling_iilidx, 
		vector<OmnThrdShellProcPtr> &runners,
		const AosRundataPtr &rdata)
{
	
	// This function creates one thread entry. 
	vector<AosIILScannerObjPtr> scanners;
	for (u32 i=0; i<mIILScanners.size(); i++)
	{
		AosIILScannerObjPtr scanner = mIILScanners[i]->clone();
		if (controlling_iilidx == i)
		{
			scanner->setCondition(start_key, start_docid, end_key, end_docid, rdata);
		}
		else
		{
			scanner->setCondition(start_key, start_docid, rdata);
		}
		scanners.push_back(scanner);
	}

	AosActJoinIILMultiThrdPtr thisptr(this, true);
	AosActJoinIILMultiThrdRunnerPtr runner = OmnNew AosActJoinIILMultiThrdRunner(
			thisptr, scanners, rdata);

	// Set the scanner listner to all the scanners
	for (u32 i=0; i<scanners.size(); i++)
	{
		scanners[i]->setListener(runner.getPtr());
	}

	runners.push_back(runner.getPtr());
	
	return true;
}


bool
AosActJoinIILMultiThrd::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	mTask = task;
	if (mIILAssembler) mIILAssembler->setTask(task);
	if (mDocAssembler) mDocAssembler->setTask(task);
	return run(sdoc, rdata);
}


bool
AosActJoinIILMultiThrd::actionFinished()
{
	bool rslt = sendAll();
	aos_assert_r(rslt, false);

	AosActionObjPtr thisptr(this, true);
	if(mTask) mTask->actionFinished(thisptr, mRundata);
			
	mEndTime = OmnGetSecond();
	OmnScreen << "join iil finished, end time:" << mEndTime << endl;
	OmnScreen << "join iil spend:" << (mEndTime - mStartTime) << endl;
	return true;
}


bool
AosActJoinIILMultiThrd::actionFailed()
{
	bool rslt = sendAll();
	aos_assert_r(rslt, false);
	
	AosActionObjPtr thisptr(this, true);
	if(mTask) mTask->actionFailed(thisptr, mRundata);
		 	 
	AosSetErrorU(mRundata, "action failed") << ": " << toString();

	mEndTime = OmnGetSecond();
	OmnScreen << "join iil failed, end time:" << mEndTime << endl;
	OmnScreen << "join iil spend:" << (mEndTime - mStartTime) << endl;
	return true;
}


bool
AosActJoinIILMultiThrd::sendAll()
{
	bool rslt;
	if (mGroupbyProc)
	{
		rslt = mGroupbyProc->procFinished(mRecord, mRundata);
		aos_assert_r(rslt, false);
		
		AosValueRslt value_rslt;
		value_rslt.setKeyValue(mRecord->getData(), 
				mRecord->getRecordLen(), false, mRecord->getDocid());
		if(mRecord->getRecordLen() > 0)
		{
			if (mIILAssembler)
			{
				rslt = mIILAssembler->appendEntry(value_rslt, mRundata);
				if(!rslt)
				{
					OmnAlarm << "record error, " << mRecord->getData() 
						<< ":" << mRecord->getRecordLen() 
						<< ":" << mRecord->getDocid() << enderr; 
				}
			}		
			
			if (mDocAssembler)
			{
				rslt = mDocAssembler->appendEntry(value_rslt, mRundata);
				if(!rslt)
				{
					OmnAlarm << "record error, " 
						<< mRecord->getData() << ":" << mRecord->getRecordLen() 
						<< ":" << mRecord->getDocid() << enderr; 
				}
			}
		}
	}

	if (mIILAssembler)
	{
		rslt = mIILAssembler->sendFinish(mRundata);
		aos_assert_r(rslt, false);
	}

	if (mDocAssembler)
	{
		rslt = mDocAssembler->sendFinish(mRundata);
		aos_assert_r(rslt, false);
	}
	return true;
}
*/
