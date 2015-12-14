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
// Modification History:
// 2015/01/08 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_UserDomainObj_h
#define AOS_SEInterfaces_UserDomainObj_h

#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Rundata/Ptrs.h"


class AosUserDomainObj: virtual public OmnRCObject
{
public:
	virtual bool isGood() const = 0;
	virtual	bool isFriendDomain(const AosUserDomainObjPtr &domain) = 0;
	virtual	OmnString getDomainObjid() const = 0;
	virtual	AosXmlTagPtr getDomainDoc(const AosRundataPtr &rdata) = 0;
	virtual	OmnString getFriendDomains() const = 0;
	virtual	bool supervisorEnabled() const = 0;
	virtual	bool isSysDomain() const = 0;
	virtual	AosXmlTagPtr getUserDomainOprArd(const AosRundataPtr &rdata) = 0;
	
	static AosXmlTagPtr createUserOprArd(
					const u64 &userid, 
					const OmnString &username, 
					const AosRundataPtr &rdata);
};
#endif
