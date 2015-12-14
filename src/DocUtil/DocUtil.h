////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 07/20/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_DocUtil_DocUtil_h
#define AOS_DocUtil_DocUtil_h

#include "Rundata/Ptrs.h"
#include "SEUtil/SeTypes.h"
#include "SEUtil/DocTags.h"
#include "Util/String.h"


class AosDocUtil
{
public:
	inline static bool isTypedAttr(const OmnString &namebuff)
	{
		return isTypedAttr(namebuff.data(), namebuff.length());
	}
	static bool isTypedAttr(const char *namebuff, const int namelen)
	{
		return (namelen > 3 && 
				namebuff[namelen-2] == eAosAttrTypeChar &&
				namebuff[namelen-3] == eAosAttrTypeChar);
	}

	inline static bool isNumAlpha(const OmnString &name, const int len)
	{
		return isNumAlpha(name.data(), len);
	}
	static bool isNumAlpha(const char *name, const int len)
	{
		return (len > 3 && name[len-3] == eAosAttrTypeChar &&
					 name[len-2] == eAosAttrTypeChar &&
					 name[len-1] == eAosAttrType_NumAlpha);
	}

	static bool isCloudidBound(
			const OmnString &siteid, 
			const OmnString &cid, 
			u64 &userid, 
			const AosRundataPtr &rdata);

	static bool	canAttrModified(const char *aname, const int len)
	{
		// Following attributes cannot be modified
		// 	AOSTAG_VERSION
		// 	AOSTAG_SITEID
		// 	AOSTAG_DOCID
		// 	AOSTAG_OBJID
		// All standard attributes start with 'zky_'. 
		//
		// if 'aname' is in the form:
		// 		AosIILName::composeAttrIILName(attribute name)
		// then we need check the following attributes:
		// 	AOSTAG_VERSION
		// 	AOSTAG_SITEID
		// 	AOSTAG_DOCID
		// 	AOSTAG_OBJID
		
		// Ketty 08/17/2011
		if (strcmp(&aname[0], AOSTAG_VERSION) == 0) return false;
		if (strcmp(&aname[0], AOSTAG_SITEID) == 0) return false;
		if (strcmp(&aname[0], AOSTAG_DOCID) == 0) return false;
		if (strcmp(&aname[0], AOSTAG_OBJID) == 0) return false;
		
		//if (len <=5 ) return true;
		//if (aname[0] == '_' &&
		//	aname[1] == 'z' &&
		//	aname[2] == 't' &&
		//	aname[3] == '1' &&
		//	aname[4] == '9') 
		//{
		//	if (len <= 9) return true;
		//	if (strcmp(&aname[5], AOSTAG_VERSION) == 0) return false;
		//	if (strcmp(&aname[5], AOSTAG_SITEID) == 0) return false;
		//	if (strcmp(&aname[5], AOSTAG_DOCID) == 0) return false;
		//	if (strcmp(&aname[5], AOSTAG_OBJID) == 0) return false;
		//	return true;
		//}
		return true;
	}
};
#endif

