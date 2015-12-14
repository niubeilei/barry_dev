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
#include "JQLStatement/JqlStmtSyncer.h"
#include "JQLExpr/Expr.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "Util/OmnNew.h"
#include "Debug/Debug.h"
#include "API/AosApi.h"
#include "StreamEngine/StreamSyncer.h"

using boost::shared_ptr;
using namespace AosConf;


AosJqlStmtSyncer::AosJqlStmtSyncer()
{
	mParms = NULL;
	mOp = JQLTypes::eOpInvalid;
}


AosJqlStmtSyncer::~AosJqlStmtSyncer()
{
	delete mParms;
}


bool
AosJqlStmtSyncer::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	if (mOp == JQLTypes::eCreate) 		 	return createSyncer(rdata);
	if (mOp == JQLTypes::eDrop) 		 	return dropSyncer(rdata);

	return true;
}

bool
AosJqlStmtSyncer::createSyncer(const AosRundataPtr &rdata)
{
	if(mName != "" && mParms->size() > 0)
	{
		AosStreamSyncer *syncer = new AosStreamSyncer();
		syncer->addToSyncerList(mName);
		for(u32 i=0; i<mParms->size(); i++)
		{
			OmnString name = (*mParms)[i]->getValue(rdata.getPtr());
			syncer->addToStreamList(name);
		}
	}
	else
		return false;

	return true;
}

bool
AosJqlStmtSyncer::dropSyncer(const AosRundataPtr &rdata)
{
	if(mName != "")
	{
		AosStreamSyncer *syncer = new AosStreamSyncer();
		bool rslt = syncer->dropSyncer(mName);
		aos_assert_r(rslt, false);
	}
	else
		return false;

	return true;
}


void
AosJqlStmtSyncer::setName(const AosExprObjPtr &name)
{
	mName = name->getValue(0);
}


AosJqlStatement *
AosJqlStmtSyncer::clone()
{
	return OmnNew AosJqlStmtSyncer(*this);
}

void 
AosJqlStmtSyncer::dump()
{
}



