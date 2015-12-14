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
#include "Actions/ActStrIILBatchInc.h"

#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/AppendFile.h"
#include "Util/Buff.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "XmlUtil/XmlTag.h"


AosActStrIILBatchInc::AosActStrIILBatchInc(const bool flag)
:
AosSdocAction(AOSACTTYPE_STRIILBATCHINC, AosActionType::eStrIILBatchInc, flag)
{
}


AosActStrIILBatchInc::AosActStrIILBatchInc(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
:
AosSdocAction(AOSACTTYPE_STRIILBATCHINC, AosActionType::eStrIILBatchInc, false)
{
	bool rslt = config(def, rdata);
	if(!rslt) OmnThrowException(rdata->getErrmsg());
}


AosActStrIILBatchInc::~AosActStrIILBatchInc()
{
}


bool
AosActStrIILBatchInc::config(
        const AosXmlTagPtr &def,
        const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);

	AosXmlTagPtr iiltag = def->getFirstChild("iilasm");
	aos_assert_rr(iiltag, rdata, false);

	mIILName = iiltag->getAttrStr(AOSTAG_IILNAME);
	aos_assert_rr(mIILName != "", rdata, false);

	mLen = iiltag->getAttrInt(AOSTAG_LENGTH, -1);
	aos_assert_r(mLen > 0, false);

	OmnString inctype = iiltag->getAttrStr(AOSTAG_INCTYPE);
	mIncType = AosIILUtil::IILIncType_toEnum(inctype);

	mInitdocid = iiltag->getAttrU64(AOSTAG_INITDOCID, 0);

	return true;
}


bool
AosActStrIILBatchInc::run(
        const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	return AosIILClientObj::getIILClient()->StrBatchInc(
		mIILName, mLen, buff, mInitdocid, mIncType, mSnapId,
		mTaskDocid, rdata);
}


AosActionObjPtr
AosActStrIILBatchInc::clone(const AosXmlTagPtr &def, const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActStrIILBatchInc(def, rdata);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}

