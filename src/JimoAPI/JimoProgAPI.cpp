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
// 2015/04/01 Created by Xia Fan
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/JimoProgAPI.h"
#include "SEInterfaces/JimoProgMgrObj.h"
#include "API/AosApi.h"

static OmnMutex     sgLock;
static AosJimoProgMgrObjPtr sgJimoProgMgr = 0;
static AosJimoProgObjPtr 	sgJimoProg;

static bool sgInit(AosRundata *rdata)
{
	sgLock.lock();
	if (sgJimoProgMgr)
	{
		sgLock.unlock();
		return true;
	}

	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, "AosJimoProgMgr", 1);
	if (!jimo)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	sgJimoProgMgr = dynamic_cast<AosJimoProgMgrObj*>(jimo.getPtr());
	if (!sgJimoProgMgr)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	jimo = AosCreateJimoByClassname(rdata, "AosJimoProgJob", 1);
	if (!jimo)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	sgJimoProg = dynamic_cast<AosJimoProgObj*>(jimo.getPtr());
	if (!sgJimoProg)
	{
		sgLock.unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}
	sgLock.unlock();

	return true;
}


namespace Jimo
{

AosJimoProgObjPtr jimoGetJimoProg(
		AosRundata *rdata, 
		const OmnString &name)
{
	if (!sgJimoProgMgr) sgInit(rdata);
	aos_assert_rr(sgJimoProgMgr, rdata, 0);

	return sgJimoProgMgr->getJimoProg(rdata, name);
}


AosJimoProgObjPtr jimoCreateJimoProg(
		AosRundata *rdata, 
		const OmnString &classname, 
		const int version)
{
	AosJimoPtr jimo = AosCreateJimoByClassname(rdata, classname, version);
	if (!jimo) 
	{
		AosLogError(rdata, true, "jimo_not_defined")
			<< AosFN("Classname") << classname << enderr;
		return 0;
	}

	AosJimoProgObjPtr prog = dynamic_cast<AosJimoProgObj *>(jimo.getPtr());
	aos_assert_rr(prog, rdata, 0);
	return prog;
}


AosJimoProgObjPtr jimoCreateLoadDataProg(
		AosRundata *rdata,
		const AosXmlTagPtr &inputds, 
		const AosXmlTagPtr &tabledoc,
		std::list<string> &fields,
		JQLTypes::OpType &op)
{
	if (!sgJimoProg) sgInit(rdata);
	aos_assert_rr(sgJimoProg, rdata, 0);

	return sgJimoProg->createLoadDataProg(rdata, inputds, tabledoc, fields, op);
};
/*
AosJimoProgObjPtr jimoRetrieveJimoProg(
		AosRundata *rdata, 
		const OmnString &name)
{
	if (!sgJimoProg) sgInit(rdata);
	aos_assert_rr(sgJimoProg, rdata, 0);

	return sgJimoProg->getJimoProg(rdata, name);
}
*/
};

