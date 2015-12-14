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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtGenericobj.h"
#include "JQLExpr/Expr.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Util/OmnNew.h"
#include "Debug/Debug.h"
#include "API/AosApi.h"

using boost::shared_ptr;
using namespace AosConf;


AosJqlStmtGenericobj::AosJqlStmtGenericobj()
{
	mParms = NULL;
	mOp = JQLTypes::eOpInvalid;
}


AosJqlStmtGenericobj::~AosJqlStmtGenericobj()
{
	delete mParms;
}


bool
AosJqlStmtGenericobj::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	/*
	if (mOp == JQLTypes::eInstall)  		return installCluster(rdata);
	if (mOp == JQLTypes::eStart)  			return startCluster(rdata);
	if (mOp == JQLTypes::eStop)  			return stopCluster(rdata);
	if (mOp == JQLTypes::eStop9)  			return stop9Cluster(rdata);
	if (mOp == JQLTypes::eRestart)  		return restartCluster(rdata);
	if (mOp == JQLTypes::eClean)  			return cleanCluster(rdata);
	if (mOp == JQLTypes::eStop_access)  	return stopAccessCluster(rdata);
	if (mOp == JQLTypes::eStart_access)  	return startAccessCluster(rdata);
	if (mOp == JQLTypes::eRestart_access)  	return restartAccessCluster(rdata);
	if (mOp == JQLTypes::eStop_html)  		return stopHtmlCluster(rdata);
	if (mOp == JQLTypes::eStart_html)  		return startHtmlCluster(rdata);
	if (mOp == JQLTypes::eRestart_html)  	return restartHtmlCluster(rdata);
	*/

	// mVerb = JQLTypes::toStr(mOp);
	aos_assert_r(mVerb != "", false);
	//AosSetErrorUser(rdata, "verb is undefined.") << enderr;
	return procGenericobj(rdata);
}


bool
AosJqlStmtGenericobj::procGenericobj(const AosRundataPtr &rdata)
{
	//OmnString msg = "";	
	//if (mClient)
	//{
	//	try{
	//		//mClient->genericVerbMgrProc(mJmoRundata, mHandler, toString(rdata)); 
	//	}
	//	catch(...)
	//	{
	//		OmnAlarm << "Error." << enderr;
	//		return false;
	//	}
	//	if (mJmoRundata.rcode == JmoReturnCode::SUCCESS)
	//	{
	//		msg << "generic verb successful";
	//		rdata->setJqlMsg(msg);
	//		return true;
	//	}
	//	msg << "generic verb failed";
	//	rdata->setJqlMsg(msg);
	//	return false;
	//}
	////AosGenericVerbMgrObjPtr mgr = AosGenericVerbMgrObj::getSelf(rdata.getPtr());
	////return mgr->proc(rdata.getPtr(), mVerb, mType, mName, *mParms);
	return true;
}


void 
AosJqlStmtGenericobj::setType(const AosExprObjPtr &type)
{
	mType = type->getValue(0);
}


void
AosJqlStmtGenericobj::setName(const AosExprObjPtr &name)
{
	mName = name->getValue(0);
}


AosJqlStatement *
AosJqlStmtGenericobj::clone()
{
	return OmnNew AosJqlStmtGenericobj(*this);
}


OmnString 
AosJqlStmtGenericobj::toString(const AosRundataPtr &rdata)
{
	OmnString str;
	str << "\"" <<mVerb << "\" " << " \"" << mType << "\" " << " \"" << mName << " \""
		<< "(";
	for (u32 i = 0; i < mParms->size(); i++ )
	{
		i == 0 
			? str << (*mParms)[i]->toString()
			: str << ", " << (*mParms)[i]->toString();
	}
	str << ");";
	return str;
}


void 
AosJqlStmtGenericobj::dump()
{
}

