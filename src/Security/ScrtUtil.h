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
//	2009/10/25	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Security_Util_h
#define Omn_Security_Util_h

#include "aosUtil/Types.h"
#include "SEUtil/Ptrs.h"
#include "Util/String.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/Ptrs.h"


extern bool AosCheckAccess(
		const int operation, 
		const AosXmlDocPtr &header,
		OmnString &errmsg, 
		AosXmlRc &errcode);

inline OmnString AosComposeSsidCookieName(const u64 &url_docid)
{
	OmnString name = "zky_ssid_";
	name << url_docid;
	return name;
}


inline OmnString AosSsidCookiePrefix()
{
	return "zky_ssid_";
}

#endif

