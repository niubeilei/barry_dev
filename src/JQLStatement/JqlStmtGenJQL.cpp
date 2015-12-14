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
// 01/17/2014	Created by Young
////////////////////////////////////////////////////////////////////////////
#include "JQLStatement/JqlStmtGenJQL.h"

#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "JSON/JSON.h"
#include "XmlUtil/XmlTag.h"
#include "SEUtil/Objid.h"
#include "SEInterfaces/GenericObjMgrObj.h"
#include "API/AosApiG.h"

#include <cassert>
#include <exception>
#include <sstream>


AosJqlStmtGenJQL::AosJqlStmtGenJQL()
{
}


AosJqlStmtGenJQL::~AosJqlStmtGenJQL()
{
}


bool
AosJqlStmtGenJQL::run(
		const AosRundataPtr &rdata, 
		const AosJimoProgObjPtr &prog)
{
	if (mConfig == "") 
	{
		rdata->setJqlMsg("ERROR: Empty JSON string");
		return false;
	}

	OmnString msg = "";

	try
	{
		JSONValue value;
		JSONReader reader;
		bool rslt = reader.parse(mConfig, value);
		aos_assert_r(rslt, false);
	}
	catch (std::exception &e)
	{
		OmnScreen << "JSONException: " << e.what() << endl;
		return false;
	}
	
	AosGenericObjMgrObjPtr mgr = AosGenericObjMgrObj::getSelf(rdata.getPtr());
	bool rslt =  mgr->proc(rdata.getPtr(), prog, mVerbName, 
			mObjectType, mObjectName, mConfig);

	string::iterator itr = mObjectType.begin();
	*itr = toupper(*itr);
	msg = rdata->getJqlMsg();
	msg << "\n";
	if ( rslt )
	{
		if(mVerbName != "run")
		{
			msg << mObjectType << " \'" << mObjectName << "\' " << mVerbName << " successfully.";
		}
		else
		{
			OmnString procType = OmnString(mObjectType);
			if(procType.toLower() == "job")
			{
				OmnString jobObjid = AosObjid::getObjidByJQLDocName(JQLTypes::eJobDoc, mObjectName);
				if(jobObjid == "") return false;
				AosXmlTagPtr job_conf = AosGetDocByObjid(jobObjid, rdata);
				aos_assert_r(job_conf, NULL);

				u64 jobId = job_conf->getAttrU64("zky_instance_id", 0);
				OmnString originName = job_conf->getAttrStr("zky_job_origin_job_name");
				if(originName != "") mObjectName = originName;
				msg << mObjectType << " \'" << mObjectName << "\' started successfully." << " Job instance id is " << jobId;
			}
			else
				msg << mObjectType << " \'" << mObjectName << "\' started successfully.";
		}
		rdata->setJqlMsg(msg);
		return true;
	}
	msg << mVerbName << " " << mObjectName << " failed";
	rdata->setJqlMsg(msg);
	return false;
}


AosJqlStatement *
AosJqlStmtGenJQL::clone()
{
	return OmnNew AosJqlStmtGenJQL(*this);
}


void 
AosJqlStmtGenJQL::dump()
{
}

