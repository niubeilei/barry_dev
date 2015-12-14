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
#include "DataJoin/DataJoin.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DataAssembler/DataAssembler.h"
#include "DataJoin/DataJoinCtlr.h"
#include "DataJoin/DataJoinType.h"
#include "DataRecord/DataRecord.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Groupby/GroupbyProc.h"
#include "Groupby/GroupbyType.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

#include "DataJoin/DataJoinCpnTownCode.h"
#include "DataJoin/DataJoinEngine.h"
//#include "DataJoin/DataJoinNetStatus.h"
#include "DataJoin/DataJoinNetStatusNew.h"
#include "DataJoin/DataJoinNorm.h"
#include "DataJoin/DataJoinOnLine.h"
#include "DataJoin/DataJoinRank.h"
#include "DataJoin/DataJoinCellFluctuate.h"
#include "DataJoin/DataJoinCellWork.h"
#include "DataJoin/DataJoinCRI.h"
#include "DataJoin/DataJoinCRIDay.h"
#include "DataJoin/DataJoinPrefixAddMod.h"
#include "DataJoin/DataJoinCarrier.h"
#include "DataJoin/DataJoinUnicomIMEI.h"
#include "DataJoin/DataJoinConvert.h"


static AosDataJoinPtr	sgJoins[AosDataJoinType::eMax+1];
static OmnMutex 		sgLock;
static bool 			sgInited = false;


AosDataJoin::AosDataJoin()
:
OmnThrdShellProc("datajoin"),
mLock(OmnNew OmnMutex()),
mSem(OmnNew OmnSem(0)),
mStartTime(0),
mEndTime(0),
mProgress(0),
mFinished(false),
mSuccess(false),
mStatus(eIdle),
mDataRetrieveSuccess(false)
{
}


AosDataJoin::AosDataJoin(
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("datajoin"),
mLock(OmnNew OmnMutex()),
mSem(OmnNew OmnSem(0)),
mStartTime(0),
mEndTime(0),
mProgress(0),
mFinished(false),
mSuccess(false),
mStatus(eIdle),
mDataRetrieveSuccess(true),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
mCtlr(ctlr)
{
}


AosDataJoin::~AosDataJoin()
{
	OmnScreen << "delete data join" << endl;
}


OmnString
AosDataJoin::toString()
{
	return "";
}

	
bool
AosDataJoin::staticInit()
{
	sgLock.lock();
	if(sgInited)
	{
		sgLock.unlock();
		return true;
	}
	
	sgJoins[AosDataJoinType::eCpnTownCode]	= OmnNew AosDataJoinCpnTownCode();
	sgJoins[AosDataJoinType::eEngine]		= OmnNew AosDataJoinEngine();
	//sgJoins[AosDataJoinType::eNet]		= OmnNew AosDataJoinNetStatus();
	sgJoins[AosDataJoinType::eNet]			= OmnNew AosDataJoinNetStatusNew();
	sgJoins[AosDataJoinType::eNorm]			= OmnNew AosDataJoinNorm();
	sgJoins[AosDataJoinType::eOnLine]		= OmnNew AosDataJoinOnLine();
	sgJoins[AosDataJoinType::eRank]			= OmnNew AosDataJoinRank();
	sgJoins[AosDataJoinType::eCellFluctuate]= OmnNew AosDataJoinCellFluctuate();
	sgJoins[AosDataJoinType::eCellWork]		= OmnNew AosDataJoinCellWork();
	sgJoins[AosDataJoinType::eCRI]			= OmnNew AosDataJoinCRI();
	sgJoins[AosDataJoinType::eCRIDay]		= OmnNew AosDataJoinCRIDay();
	sgJoins[AosDataJoinType::ePrefixAddMod]	= OmnNew AosDataJoinPrefixAddMod();
	sgJoins[AosDataJoinType::eCarrier]		= OmnNew AosDataJoinCarrier();
	sgJoins[AosDataJoinType::eUnicomIMEI]	= OmnNew AosDataJoinUnicomIMEI();
	sgJoins[AosDataJoinType::eConvert]		= OmnNew AosDataJoinConvert();

	sgInited = true;
	sgLock.unlock();
	return true;
}
	

AosDataJoinPtr	
AosDataJoin::createDataJoin(
		const AosXmlTagPtr &def,
		const AosDataJoinCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, 0);
	aos_assert_r(ctlr, 0);

	staticInit();

	AosDataJoinType::E type = AosDataJoinType::toEnum(def->getAttrStr(AOSTAG_TYPE, "norm"));
	bool rslt = AosDataJoinType::isValid(type);
	aos_assert_r(rslt, 0);

	AosDataJoinPtr join = sgJoins[type];
	aos_assert_r(join, 0);

	join = join->create(def, ctlr, rdata);
	aos_assert_r(join, 0);

	return join;
}


bool
AosDataJoin::checkConfigStatic(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	staticInit();

	AosDataJoinType::E type = AosDataJoinType::toEnum(def->getAttrStr(AOSTAG_TYPE, "norm"));
	bool rslt = AosDataJoinType::isValid(type);
	aos_assert_r(rslt, false);

	AosDataJoinPtr join = sgJoins[type];
	aos_assert_r(join, false);

	rslt = join->checkConfig(def, task, rdata);
	aos_assert_r(rslt, false);

	return true;
}


AosDataAssemblerObjPtr
AosDataJoin::createIILAsm(
		const AosXmlTagPtr &iilasm,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	if (!iilasm) return 0;
	aos_assert_r(task, 0);

	OmnString dataColId = iilasm->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
	aos_assert_r(dataColId != "", 0);
	
	AosXmlTagPtr dataColTag = task->getDataColTag(dataColId, rdata);
	aos_assert_r(dataColTag, 0);
	
	AosDataAssemblerObjPtr iil_asm = AosCreateIILAssembler(rdata.getPtrNoLock(), dataColId, task, dataColTag);
	aos_assert_r(iil_asm, 0);

	return iil_asm;
}


bool
AosDataJoin::dataRetrieved(
		const AosIILScannerObjPtr &scanner,
		const AosRundataPtr &rdata)
{
	mDataRetrieveSuccess = true;
	mSem->post();
	return true;
}


bool 
AosDataJoin::dataRetrieveFailed(
		const AosIILScannerObjPtr &scanner, 
		const AosRundataPtr &rdata)
{
	mDataRetrieveSuccess = false;
	mSem->post();
	return false;
}

	
bool
AosDataJoin::noMoreData(
		const AosIILScannerObjPtr &scanner,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosDataJoin::procFinished()
{
	return true;
}

