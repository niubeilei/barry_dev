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
// 01/08/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTorUtil_StAccessRole_h
#define AosSengTorUtil_StAccessRole_h

#include "API/AosApiS.h"
#include "Rundata/Ptrs.h"
#include "Accesses/AccessType.h"
#include "SengTorUtil/StAccess.h"
#include "Thread/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/HashUtil.h"


class AosStAccessRole : public AosStAccess
{
	OmnDefineRCObject;

private:
	bool		mDeny;

public:
	AosStAccessRole(const bool flag)
	:
	AosStAccess(AOSACCESSTYPE_ROLE, flag)
	{
	}
	~AosStAccessRole() {}

	// virtual bool checkAccess(
	// 				const AosStDocPtr &local_doc,
	// 				bool &granted, 
	// 				bool &denied,
	// 				const AosSengTestThrdPtr &thread);
	virtual bool checkAccessByOpr(
					const AosStDocPtr &local_doc,
					const AosXmlTagPtr &def,
					bool &granted, 
					bool &denied,
					const AosSengTestThrdPtr &thread);
	virtual OmnString getXmlStr(const OmnString &tname, const AosSengTestThrdPtr &thread);
};
#endif

