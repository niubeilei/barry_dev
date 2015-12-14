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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoProg/JimoProgGloble.h"

#include "API/AosApi.h"
#include "JimoProg/JimoProgLoadData.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Mutex.h"


/*
extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosJimoProg_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosJimoProg(version);
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

*/


AosJimoProgGloble::AosJimoProgGloble()
:
AosJimoProg(OmnNew OmnMutex())
{
}


/*
AosJimoProgGloble::AosJimoProgGloble(const int version)
:
AosJimoProgObj(version),
mLock(OmnNew OmnMutex())
{
}
*/


AosJimoProgGloble::~AosJimoProgGloble()
{
}

AosJimoProgObjPtr
AosJimoProgGloble::createJimoProg(
		const AosXmlTagPtr &def,
		AosRundata *rdata)
{
	OmnNotImplementedYet;
	return 0;
}


bool 
AosJimoProgGloble::saveLogicDoc(
		AosRundata *rdata, 
		const OmnString &objid, 
		const OmnString &conf)
{
	AosXmlTagPtr doc = AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
	if(doc)
	{
		OmnString msg = "";
		msg << "dataProc maybe exist!";
		rdata->setJqlMsg(msg);
		return false;
	}
	return AosCreateDoc(conf, true, rdata);	
}

AosXmlTagPtr 
AosJimoProgGloble::getLogicDoc(
		AosRundata *rdata, 
		const OmnString &objid)
{
	/*if (mLogicConfs.count(objid)) 
	{
		//return AosXmlParser::parse(mLogicConfs[objid] AosMemoryCheckerArgs);
		return mLogicConfs[objid];
	}*/
	return AosDocClientObj::getDocClient()->getDocByObjid(objid, rdata);
}

