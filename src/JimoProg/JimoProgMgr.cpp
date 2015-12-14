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
// 2015/04/01 Created by Xia Fan
////////////////////////////////////////////////////////////////////////////
#include "JimoProg/JimoProgMgr.h"
#include "JimoProg/JimoProg.h"
#include "SEInterfaces/JimoProgMgrObj.h"




extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosJimoProgMgr_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosJimoProgMgr(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}






AosJimoProgMgr::AosJimoProgMgr(const int version)
:
AosJimoProgMgrObj(version),
mLock(OmnNew OmnMutex())
{
	mJimoType = AosJimoType::eJimoProgMgr;
}

AosJimoProgMgr::~AosJimoProgMgr()
{
}


AosJimoProgObjPtr 
AosJimoProgMgr::getJimoProg(AosRundata *rdata, const OmnString &name)
{
	mLock->lock();
	itr_t itr = mJimoProgMap.find(name);
	if (itr == mJimoProgMap.end())
	{
		mLock->unlock();
		AosJimoProgObjPtr prog = retrieveJimoProg(rdata, name);
		if (!prog)
		{
			AosLogError(rdata, true, "JimoProg not found")
				<< AosFN("ProgName") << name << enderr;
			return 0;
		}

		mLock->lock();
		mJimoProgMap[name] = prog;
		mLock->unlock();
		return prog;
	}
	
	mLock->unlock();
	return itr->second;
}


AosJimoPtr 
AosJimoProgMgr::cloneJimo() const
{
	return OmnNew AosJimoProgMgr(*this);
}



AosJimoProgObjPtr 
AosJimoProgMgr::retrieveJimoProg(AosRundata *rdata, const OmnString &name) const
{
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByKey(
			rdata, AOS_SYSTABLENAME_JIMOPROGS, "name", name);
	if (!doc)
	{
		AosLogError(rdata, true, "jimo_prog_not_found")
			<< AosFN("ProgName") << name << enderr;
		return 0;
	}

	AosJimoProgObjPtr prog_new = OmnNew AosJimoProg();
	AosJimoProgObjPtr prog = prog_new->createJimoProg(doc, rdata);
	if (!prog)
	{
		AosLogError(rdata, true, "invalid_jimo_prog_config")
			<< AosFN("JimoProgCond") << doc << enderr;
		return 0;
	}

	return prog;
}


