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
//
// Modification History:
// 03/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ImportData/ImportDataThrd.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "ImportData/ImportDataCtlr.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataCacherObj.h"
#include "Util/ReadFile.h"
#include "Util/UtUtil.h"
#include "Util/ValueRslt.h"
#include "XmlUtil/XmlTag.h"

static int smBuffSize = AosImportDataThrd::eDftBuffSize;
static int smReadSize = AosImportDataThrd::eDftReadSize;


AosImportDataThrd::AosImportDataThrd(
		const AosDataCacherObjPtr &cacher,
		const AosImportDataCtlrPtr &ctlr,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("import_data"),
mStartTime(0),
mEndTime(0),
mFinished(false),
mSuccess(false),
mCtlr(ctlr),
mDataCacher(cacher),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
mNeedCreateDocid(false)
{
}


AosImportDataThrd::~AosImportDataThrd()
{
}


bool
AosImportDataThrd::StaticInit(const AosXmlTagPtr &conf)
{
	// <task AOSTAG_BUFF_SIZE="xxx"
	// 		 AOSTAG_READ_SIZE="xxx"/>
	aos_assert_r(conf, false);
	smBuffSize = conf->getAttrInt(AOSTAG_BUFF_SIZE, eDftBuffSize);
	if (smBuffSize < eDftBuffSize) smBuffSize = eDftBuffSize;

	smReadSize = conf->getAttrInt(AOSTAG_READ_SIZE, eDftReadSize);
	if (smReadSize < eMinReadSize) smReadSize = eMinReadSize;

	return true;
}


OmnString	
AosImportDataThrd::getStatusRecord() const
{
	// It creates the status record:
	// 	<record AOSTAG_FILENAME="xxx"
	// 		AOSTAG_START_TIME="xxx"
	// 		AOSTAG_DURATION="xxx"
	// 		AOSTAG_END_TIME="xxx">
	// 	</record>
	OmnString docstr;
	docstr << "<record "
		   << AOSTAG_STATUS << "=\"" << mSuccess << "\" "
		   << AOSTAG_START_TIME << "=\"" << mStartTime << "\" "
		   << AOSTAG_END_TIME << "=\"" << mEndTime << "\" "
		   << AOSTAG_DURATION << "=\"" << mEndTime - mStartTime << "\" />";
	return docstr;
}

