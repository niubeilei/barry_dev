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
// This action get a function from a dll lib,
// then run the function and get the return value.
//
// Modification History:
// 2012/05/11	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActStrIILBatchAdd.h"


#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/AppendFile.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


AosActStrIILBatchAdd::AosActStrIILBatchAdd(const bool flag)
:
AosSdocAction(AOSACTTYPE_STRIILBATCHADD, AosActionType::eStrIILBatchAdd, flag)
{
}


AosActStrIILBatchAdd::AosActStrIILBatchAdd(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_STRIILBATCHADD, AosActionType::eStrIILBatchAdd, false)
{
	bool rslt = config(def, rdata);
	if(!rslt) OmnThrowException(rdata->getErrmsg());
}


AosActStrIILBatchAdd::~AosActStrIILBatchAdd()
{
}


bool
AosActStrIILBatchAdd::config(
        const AosXmlTagPtr &def,
        const AosRundataPtr &rdata)
{
	// The smartdoc format is:
	// 	<sdoc ...>
	// 		<iil AOSTAG_IILNAME="xxx" AOSTAG_LENGTH="xxx"/>
	// 	</sdoc>
	aos_assert_r(def, false);

	AosXmlTagPtr iiltag = def->getFirstChild("iilasm");
	aos_assert_rr(iiltag, rdata, false);

	mIILName = iiltag->getAttrStr(AOSTAG_IILNAME);
	aos_assert_rr(mIILName != "", rdata, false);

	mLen = iiltag->getAttrInt(AOSTAG_LENGTH, -1);
	aos_assert_r(mLen > 0, false);
	
	return true;
}


bool
AosActStrIILBatchAdd::run(
        const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	return AosIILClientObj::getIILClient()->StrBatchAdd(
		mIILName, mLen, buff, 0, mSnapId, mTaskDocid, rdata);
}


AosActionObjPtr
AosActStrIILBatchAdd::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActStrIILBatchAdd(def, rdata);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

