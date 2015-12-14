//
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
// 03/28/2011: Created by Phnix
////////////////////////////////////////////////////////////////////////////
#include "SEModules/CopyVpd.h"

#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/XmlTag.h"


OmnString 
AosCopyVpd::getNextVpdObjid(
		const OmnString &prefix_type,
		const OmnString &oldObjid,
		OmnString &strAdd,
		const AosRundataPtr &rdata)
{
	 // prefix_type: 
	 // 1. prefix style: oldObjid, add prefix to oldObjid then create a new Objid
	 // 2. post style: add oldObjid_post+num, which the num is the times of copy..
	 // 3. oldObjid add seqno, then create a new Objid;
	OmnString newObjid = "";
	AosCopyType::E eCopyType = AosCopyType::toEnum(prefix_type);

	switch(eCopyType)
	{
	case AosCopyType::ePrefix:
		 newObjid << strAdd << "_" << oldObjid;
		 return newObjid;

	case AosCopyType::eSuffix:
		 newObjid << oldObjid << "_" << strAdd;
		 return newObjid;

	case AosCopyType::eSeqno:
		 return attachSeqno(oldObjid);

	default:
		 rdata->setError() << "Unrecognized prefix type: " << prefix_type;
		 OmnAlarm << rdata->getErrmsg() << enderr;
		 return "";
	}
	return newObjid;
}


OmnString 
AosCopyVpd::attachSeqno(const OmnString &objid)
{
	// This function checks whether 'objid' is in the format:
	// 	xxx_ddd
	// where 'ddd' is a digital string. If not, it attaches: "_0" to 'objid'. 
	// Otherwise, it increments ddd by one. 
	int pos = objid.find('_', true);
	if (pos < 0)
	{
		// There is no '_'. 
		OmnString bb = objid;
		bb << "_0";
		return bb;
	}

	// Found a underscore. Retrieve the substr after the underscore.
	OmnString digitstr = objid.substr(pos+1);
	if (digitstr == "" || !digitstr.isDigitStr())
	{
		OmnString bb = objid;
		bb << "_0";
		return bb;
	}

	int seqno = atoi(digitstr.data());
	if (seqno < 0)
	{
		OmnString bb = objid;
		bb << "_0";
		return bb;
	}

	OmnString bb = objid.substr(0, pos);
	bb << "_" << seqno++;
	return bb;
}

