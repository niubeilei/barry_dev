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
// 2012/07/30	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Actions/ActDataJoin.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataJoin/DataJoinCtlr.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util/DataTable.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEInterfaces/TaskDataObj.h"


AosActDataJoin::AosActDataJoin(const bool flag)
:
AosSdocAction(AOSACTTYPE_DATAJOIN, AosActionType::eDataJoin, flag)
{
}


AosActDataJoin::~AosActDataJoin()
{
}


bool
AosActDataJoin::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
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

	// 1. Issue a request to retrieve data for every IIL in 'mIIL'. 
	if (!sdoc)
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	if(!task || !task->getTaskData())
	{
		AosSetError(rdata, "eMissingSmartDoc");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	mTask = task;

	AosXmlTagPtr join_tag = sdoc->getFirstChild("datajoin");
	aos_assert_r(join_tag, false);

	AosDataJoinListenerPtr thisptr(this, true);
	AosDataJoinCtlrPtr ctlr = AosDataJoinCtlr::createDataJoinCtlr(join_tag, thisptr, task, rdata);
	aos_assert_r(ctlr, false);

	bool rslt = ctlr->start(rdata);
	if (!rslt)
	{
		joinFinished(false, 0, rdata);
	}
	return rslt;
}


bool
AosActDataJoin::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	
	AosXmlTagPtr join_tag = def->getFirstChild("datajoin");
	aos_assert_r(join_tag, false);

	bool rslt = AosDataJoinCtlr::checkConfig(join_tag, task, rdata);
	aos_assert_r(rslt, false);

	return true;
}


AosActionObjPtr
AosActDataJoin::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	aos_assert_r(def, 0);
	
	try
	{
		return OmnNew AosActDataJoin(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


void 
AosActDataJoin::joinFinished(
		const bool rslt, 
		const AosXmlTagPtr &status_doc,
		const AosRundataPtr &rdata)
{
	OmnScreen << "join finished: " << rslt << endl;
	if (!mTask)
	{
		OmnScreen << "join data finished: " << rslt << endl;
		return;
	}

	AosActionObjPtr thisptr(this, true);
	if (!rslt)
	{
		mTask->actionFailed(thisptr, rdata);
	}
	else
	{
		mTask->actionFinished(thisptr, rdata);
	}
}

#endif
