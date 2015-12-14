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
// 2013/05/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartDocDyn/SdocCreateFile.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "TransBasic/Trans.h"
#include "Util/DirDesc.h"
#include "XmlUtil/SeXmlParser.h"


bool AosSdocCreateFileFunc(const AosRundataPtr &rdata) 
{
	AosXmlTagPtr sdoc = rdata->getSdoc();
	aos_assert_rr(sdoc, rdata, false);

	OmnString dir = sdoc->getAttrStr("directory");
	if (dir == "")
	{
		AosSetErrorU(rdata, "missing_dir");
		return false;
	}

	int phy_id = sdoc->getAttrInt(AOSTAG_PHYSICALID, -1);
	if (!AosIsValidPhysicalIdNorm(phy_id))
	{
		AosSetErrorU(rdata, "invalid_physical_id") << ": " << phy_id;
		return false;
	}

	if (AosIsPhysicalIdLocal(phy_id))
	{
		return AosSdocCreateFile::createFile(rdata, sdoc, phy_id, dir);
	}
	
	AosTransPtr trans = OmnNew AosTransSdocCreateFile(rdata, phy_id, sdoc);
	AosSendTrans(rdata, trans);
	return true;
}


bool 
AosSdocCreateFile::createFile(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &sdoc, 
		const int orig_phy_id, 
		const OmnString &dir)
{
	try
	{
		OmnString fname;
		OmnDirDesc dir_desc(dir);
		while (dir_desc.getNextFile(fname))
		{
			OmnScreen << "File: " << fname << endl;
		}
	}

	catch (...)
	{
		AosSetErrorU(rdata, "directory_not_exist") << ": " << dir;
		return false;
	}

	return true;
}


// The AosTransSdocCreateFile transaction
AosTransSdocCreateFile::AosTransSdocCreateFile(const bool regflag)
:
AosTaskTrans(AosTransType::eSendBitmapInstrs, regflag)
{
}


AosTransSdocCreateFile::AosTransSdocCreateFile(
		const AosRundataPtr &rdata, 
		const int phy_id, 
		const AosXmlTagPtr &sdoc)
:
AosTaskTrans(AosTransType::eSendBitmapInstrs, phy_id, false, false),
mReqServerId(0),
mSdoc(sdoc)
{
}


AosTransSdocCreateFile::~AosTransSdocCreateFile()
{
}


bool
AosTransSdocCreateFile::serializeTo(const AosBuffPtr &buff)
{
	// Its format is:
	// 		self server id		int	
	// 		sdoc				OmnString
	aos_assert_r(mSdoc, false);
	buff->setInt(AosGetSelfServerId());
	buff->setOmnStr(mSdoc->toString());
	return true;
}


bool
AosTransSdocCreateFile::serializeFrom(const AosBuffPtr &buff)
{
	// Its format is:
	// 		self server id		int	
	// 		recv server id		int
	// 		sdoc				OmnString
	mReqServerId = buff->getInt(-1);
	OmnString ss = buff->getOmnStr("");
	mSdoc = AosXmlParser::parse(ss AosMemoryCheckerArgs);
	return false;
}


AosTransPtr
AosTransSdocCreateFile::clone()
{
	return OmnNew AosTransSdocCreateFile(false);
}


bool 
AosTransSdocCreateFile::proc()
{
	OmnNotImplementedYet;
	return true;
}

